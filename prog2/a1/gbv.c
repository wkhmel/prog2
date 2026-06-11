#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gbv.h"
#include "util.h"

typedef struct {
    int qtd_doc; /*quantos documentos esse superbloco tem*/
    long offset; /*em qual offset começa a área do diretório*/
} Superblock;


int gbv_create(const char *filename) {
	FILE *arquivo = fopen(filename, "wb");
        
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return -1;
    }
        
    Superblock sb;
    sb.qtd_doc = 0; /* começa sem documentos */
    sb.offset = sizeof(Superblock); /* offset do diretorio */

    /* criei essa variável pra n ter que fechar o arquivo dentro e fora do if */
	int x = 0;

    /* escrevendo no arquivo as informações sobre o superbloco */
    if (!(fwrite(&sb, sizeof(Superblock), 1, arquivo))) {
	    fprintf(stderr, "Erro ao escrever no arquivo.\n");
        x = -1;
    }

    fclose(arquivo); /* fechando o arquivo */
    return x;
}

int gbv_open(Library *lib, const char *filename) {
	if (!lib || !filename) {
		fprintf(stderr, "Ponteiro nulo.\n");
		return -1;
	}

    FILE *arquivo = fopen(filename, "rb"); /* abrindo o documento */

    if (!arquivo) {
        if (gbv_create(filename) != 0) { /* se a biblioteca ainda n existe eu tento criá-la */
            fprintf(stderr, "Erro ao criar o arquivo.\n");
            return -1;
        }
        /* depois de criada, ainda assim pode dar erro */
        arquivo = fopen(filename, "rb");
		
        if (!arquivo) {
            fprintf(stderr, "Erro ao abrir o arquivo.\n");
            return -1;  
	    }
    }

	Superblock sb;
    /* o fread copia o q está no arquivo para sb e ponteiro vai pra depois do superbloco */
   /* tem q dar a mesma qtd de arquivos q eu li (1) */
    if (fread(&sb, sizeof(Superblock), 1, arquivo) != 1) {
		fprintf(stderr, "Erro ao ler o arquivo.\n");
        fclose(arquivo);
		return -1;
	} 
    
	/* vai c o ponteiro do início do arquivo até onde o diretório começa */
    fseek(arquivo, sb.offset, SEEK_SET); 
    
    lib->count = sb.qtd_doc;        
    
    if (lib->count > 0) {
    	/* alocando o espaço pra todos os docs q precisa ter na livraria */
		lib->docs = malloc(sizeof(Document)*sb.qtd_doc); /* podia ser de qtd lib->count tbm mas acho q tanto faz */
		
		if (!lib->docs) {
			fprintf(stderr, "Erro ao alocar memoria.\n");
			fclose(arquivo); /* sempre fechar arquivo antes de dar return!! */
        	return -1;
		}

        /* do arquivo p lib->docs */
		if (fread(lib->docs, sizeof(Document), sb.qtd_doc, arquivo) != sb.qtd_doc) {
        	fprintf(stderr, "Erro ao ler o arquivo.\n");
			fclose(arquivo);
        	free(lib->docs);
        	return -1;
    	}

    } else {
        lib->docs = NULL; /* se n tiver nenhum documento, o vetor de docs é nulo */
    }

    fclose(arquivo);
    return 0;
}

int gbv_find(const Library *lib, const char *procurado) {

    for (int i = 0; i < lib->count; i++) {
        if (strcmp(lib->docs[i].name, procurado) == 0) {
            return i;
        }
	}
    
    return -1;
}

int gbv_add(Library *lib, const char *archive, const char *docname) {
    if (!lib || !archive || !docname) {
        fprintf(stderr, "Ponteiro nulo.\n");
        return -1;
	}

	/* comparando se os dois arquivos binarios são iguais, n só se têm o mesmo nome */
	if (strcmp(docname, archive) == 0) {
    	fprintf(stderr, "Voce nao pode inserir o arquivo container '%s' dentro dele mesmo!\n", archive);
    	return -1;
	}

	FILE *doc_insercao = fopen(docname, "rb"); /* abrindo arquivo q quero inserir */

    if (!doc_insercao) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return -1;
    }

    FILE *destino = fopen(archive, "rb+"); /* destino q vai receber a inserção, p leitura e alteração (+) */
    
    if (!destino) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        fclose(doc_insercao);
        return -1;
    }

    Superblock sb;

    /* tentando colocar o primeiro bloco do destino de tamanho "Superblock" para &sb */
    if (fread(&sb, sizeof(Superblock), 1, destino) != 1) {
		fprintf(stderr, "Erro ao ler o arquivo.\n");
        fclose(doc_insercao);
		fclose(destino);
        return -1;
    }

    fseek(destino, sb.offset, SEEK_SET); 
    fseek(doc_insercao, 0, SEEK_END);
    
    /* tamanho do documento vai ser o comprimento do fseek */
    long tam_arquivo = ftell(doc_insercao);
    /* resetando para o começo */    
    rewind(doc_insercao);

    /* declaramos o buffer */
    unsigned char buffer[BUFFER_SIZE];
    size_t lidos; /* qtd de bytes lidos em cada fread */

    while ((lidos = fread(buffer, 1, BUFFER_SIZE, doc_insercao)) > 0) {
        if (fwrite(buffer, 1, lidos, destino) != lidos) {
            fprintf(stderr, "Erro ao escrever no arquivo.\n");
            return -1;
        }
    }

    /* cria um vetor auxiliar que recebe realloc para aumentar o seu tamanho */
    /* não alteramos diretamente o lib->docs para não o perdermos caso a realocação não dê certo */
    Document *aux = realloc(lib->docs, sizeof(Document)*(lib->count + 1));

    if (!aux) {
        fprintf(stderr, "Erro ao criar documento.\n");        
        fclose(doc_insercao);
        fclose(destino);
        return -1;
    }

    /* se deu certo o realloc, posso atualizar o lib->docs para o auxiliar q criei */
    lib->docs = aux;
    
    int idx = gbv_find(lib, docname); /* procurando índice onde docname está no vet de nomes*/
    
    /* substituindo arquivos com o mesmo nome pelos mais recentes (fiz no final) */    
    if (idx != -1) {
        for (int i = idx; i < lib->count - 1; i++) {
            lib->docs[i] = lib->docs[i + 1];            
        }
        lib->count--;
    }

    /* passando todos os arquivos do docname para o último documento da biblioteca */
    strncpy(lib->docs[lib->count].name, docname, MAX_NAME - 1);
    lib->docs[lib->count].name[MAX_NAME - 1] = '\0'; /* tem q terminar com /0 */    
    lib->docs[lib->count].size = tam_arquivo;
    lib->docs[lib->count].offset = sb.offset;
    lib->docs[lib->count].date = time(NULL); /* data atual */
    
    lib->count++; /* agora temos mais elementos (ou a mesma qtd se tivermos feito substituição )*/

	sb.qtd_doc = lib->count;
    sb.offset = ftell(destino); /* usando o fseek da linha 152 */
    
    if (fwrite(lib->docs, sizeof(Document), lib->count, destino) != lib->count) {
        fprintf(stderr, "Erro ao escrever no arquivo.\n");
        fclose(doc_insercao);
        fclose(destino);
        return -1;
    }

    rewind(destino);
    int x = 0;

    /* escrevendo no sb as informaçoes q estão no endereço de destino */
    if (fwrite(&sb, sizeof(Superblock), 1, destino) != 1) {
        fprintf(stderr, "Erro ao escrever no arquivo.\n");
        x = -1;
    }

    /* fechando os arquivos */
    fclose(doc_insercao);
    fclose(destino);
    return x;
}

int gbv_remove(Library *lib, const char *archive, const char *docname) {
    if (!lib || !docname) {
        fprintf(stderr, "Ponteiro nulo.\n");
        return -1;
    }

    int idx = gbv_find(lib, docname); /* procurando em q índice ta o doc q quero excluir*/

    if (idx == -1) { /* caso de o elemento a ser excluído do arquivo n estar no arquivo */
        fprintf(stderr, "Arquivo %s nao encontrado.\n", docname);
        return -1;   
    }

    /* vou substituir o elemento a ser removido pelo q está na frente e assim por diante */
    for (int i = idx; i < lib->count - 1; i++) {
        lib->docs[i] = lib->docs[i + 1];
    }

    /* vou tirar o docname do disco pra ele n continuar aparecendo no view*/
    FILE *arquivo = fopen(archive, "rb+"); 

    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return -1;
    }

    Superblock sb;
    if (fread(&sb, sizeof(Superblock), 1, arquivo) != 1) {
        fprintf(stderr, "Erro ao ler o arquivo.\n");
        fclose(arquivo);
        return -1;
    }

    sb.qtd_doc = lib->count - 1;
    fseek(arquivo, sb.offset, SEEK_SET); /* vou até onde começa o diretório de destino */

    if (fwrite(lib->docs, sizeof(Document), sb.qtd_doc, arquivo) != sb.qtd_doc) {
        fprintf(stderr, "Erro ao escrever no arquivo.\n");
        fclose(arquivo);
        return -1;
    }

    int x = 0;
    rewind(arquivo);

    if (fwrite(&sb, sizeof(Superblock), 1, arquivo) != 1) {
        fprintf(stderr, "Erro ao escrever no arquivo.\n");
        x = -1;
    }

    lib->count--;
    fclose(arquivo);
    return x;
}

int gbv_list(const Library *lib) {
    if (!lib) {
        fprintf(stderr, "Ponteiro nulo\n");
        return -1;
    }

    if (lib->count == 0) {
        printf("Nao ha elementos na biblioteca.\n");
        return 0;
    }

	char data[32];
	
    printf("Quantidade total de documentos: %d\n", lib->count);
	printf("\n");

    for (int i = 0; i < lib->count; i++) {
		format_date(lib->docs[i].date, data, sizeof(data));
        printf("Documento atual: %d -- ", i);
        printf("Nome: %s -- ", lib->docs[i].name);
        printf("Tamanho: %ld -- ", lib->docs[i].size);
        printf("Data de insercao: %s -- ", data);
        printf("Posicao (offset): %ld\n", lib->docs[i].offset);
        printf("-----------------------------------------------------------------------------------------------------\n");
    }

    return 0;
}

int gbv_view(const Library *lib, const char *archive, const char *docname) {
    if (!lib || !docname) {
        fprintf(stderr, "Ponteiro nulo.\n");
        return -1;
    }

    int idx = gbv_find(lib, docname);

    if (idx == -1) {
        fprintf(stderr, "Arquivo %s nao encontrado.\n", docname);
        return -1;
    }

    FILE *arquivo = fopen(archive, "rb");

    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return -1;
    }

    long bytes_lidos = 0;
	
	long falta_ler;
	int lidos;
	int ler_agr;
    
    char select; /* opção escolhida pelo usuário (p,q,n)*/
    unsigned char buffer[BUFFER_SIZE + 1];

    while (1) {
        /* posição atual = inicio do documento + qtd de bytes lidos */
        long posicao = lib->docs[idx].offset + bytes_lidos;
        
        fseek(arquivo, posicao, SEEK_SET); /* coloca o ponteiro do arquivo na posicao atual */

        falta_ler = lib->docs[idx].size - bytes_lidos; /*tamanho total menos qto falta ler */

		if (falta_ler > BUFFER_SIZE)
			ler_agr = BUFFER_SIZE; /* lê só um bloco de no máx buffer_size de cada vez */
		else
			ler_agr = (int)falta_ler;
		

        memset(buffer, 0, sizeof(buffer)); /* limpa o buffer */    
        
        if ((lidos = fread(buffer, 1, ler_agr, arquivo)) != (size_t)ler_agr) {
            fprintf(stderr, "Erro ao ler o arquivo.\n");
            break;
        }

        buffer[lidos] = '\0';

        printf("\n--- Visualizando: %s (%ld bytes de %ld) ---\n", docname, bytes_lidos + lidos, lib->docs[idx].size);        
        printf("%s\n", buffer);
        printf("-------------------------------------------\n");
        
        /* so dá a opção se tiver mais bytes para serem lidos */
		if (bytes_lidos + lidos < lib->docs[idx].size)
			printf("(n) -> proximo bloco; ");

        /* essa opção só é mostrada se ainda há coisas a serem lidas */
		if (bytes_lidos > 0)
       		printf("(p) -> bloco anterior; ");		
        
        printf("(q) -> sair da visualizacao\n");
        
        scanf(" %c", &select);

        if (select == 'q')
            break;
    		
        if (select == 'n') {
            /* tem que observar se ainda tem bytes a serem lidos, senão eu aviso q já estamos no fim */
			if (bytes_lidos + BUFFER_SIZE < lib->docs[idx].size)
            	bytes_lidos = bytes_lidos + BUFFER_SIZE;
			else {
				printf("\nVoce ja esta no ultimo bloco do arquivo.\n");
			}
		}
			
        /* aqui eu reduzo os bytes somente se eu já não estiver no primeiro bloco*/
        if (select == 'p') {
			if (bytes_lidos >= BUFFER_SIZE)
            	bytes_lidos = bytes_lidos - BUFFER_SIZE;
			else
				printf("\nVoce ja esta no primeiro bloco do arquivo.\n");
		}

        if (select != 'p' && select != 'n' && select != 'q')
            printf("\nEntrada invalida.\n");
    }

    fclose(arquivo);
    return 0;
}

int ordem_alfabetica(const void *a, const void *b) {
    /*convertendo o ponteiro do tipo void (generico pra comparação) p tipo Document */
    const Document *docA = (const Document *)a;
    const Document *docB = (const Document *)b;

    /* dá zero se forem iguais, valor negativo se o primeiro for menor q o segundo e positivo caso contrario */
    return strcmp(docA->name, docB->name);
}

int ordem_tamanho(const void *a, const void *b) {
    const Document *docA = (const Document *)a;
    const Document *docB = (const Document *)b;

	/* estabelecendo uma ordem entre a e b, de 0 se ambos forem iguais, 1 se o primeiro for maior e -1 c.c. */
    if (docA->size == docB->size)
		return 0;
	else if (docA->size > docB->size)
		return 1;
	else
		return -1;
}

int ordem_cronologica(const void *a, const void *b) {
    const Document *docA = (const Document *)a;
    const Document *docB = (const Document *)b;

	/* mesma ideia do ordem_tamanho */
    if (docA->date == docB->date)
		return 0;
	else if (docA->date > docB->date)
		return 1;
	else
		return -1;
}

int gbv_order(Library *lib, const char *archive, const char *criteria) {
    if (!lib || !criteria) {
        fprintf(stderr, "Ponteiro nulo.\n");
        return -1;
    }

    /* vendo se o cara q escreveu colocou algum criterio valido (tem q escrever exatamente "nome" ou "tamanho" ou "data")*/
    if (strcmp(criteria, "nome") == 0) { 
        /* fazendo um quicksort de acordo com a função de ordenação e passando o resultado para o lib->docs */
        qsort(lib->docs, lib->count, sizeof(Document), ordem_alfabetica);
    } else if (strcmp(criteria, "tamanho") == 0) {
        qsort(lib->docs, lib->count, sizeof(Document), ordem_tamanho);
    } else if (strcmp(criteria, "data") == 0) {
        qsort(lib->docs, lib->count, sizeof(Document), ordem_cronologica);
    } else {
        fprintf(stderr, "Criterio de ordenacao invalido.\n");
        return -1;
    }

    FILE *arquivo = fopen(archive, "rb+"); /* alterar o diretorio*/

    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return -1;
    }

    Superblock sb;
    if (fread(&sb, sizeof(Superblock), 1, arquivo) != 1) {
        fprintf(stderr, "Erro ao ler o arquivo.\n");
        fclose(arquivo);
        return -1;
    }

    sb.qtd_doc = lib->count;
    
    fseek(arquivo, sb.offset, SEEK_SET); 

    if (fwrite(lib->docs, sizeof(Document), lib->count, arquivo) != lib->count) {
        fprintf(stderr, "Erro ao escrever no arquivo.\n");
        fclose(arquivo);
        return -1;
    }

    rewind(arquivo);
    int x = 0;

    if (fwrite(&sb, sizeof(Superblock), 1, arquivo) != 1) {
        fprintf(stderr, "Erro ao escrever no arquivo.\n");
        x = -1;
    }

    fclose(arquivo);
    return x;

}
