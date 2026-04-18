#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gbv.h"
#include "util.h"

typedef struct {
	int qtd_doc; // quantos documentos esse superbloco tem
    long offset; // em qual offset começa a área do diretório
} Superblock;

/* cria um arquivo container e coloca o espaço necessário de um superbloco p/ inicializar */
/* retorna 0 em sucesso e -1 em erro */
int gbv_create(const char *filename) {
	FILE *arquivo = fopen(filename, "wb"); /* abre arquivo para gravação */
        
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return -1;
    }

    Superblock sb;
    sb.qtd_doc = 0; /* começa sem documentos */
    sb.offset = sizeof(Superblock); /* ele vem dps do espaço ocupado pelo superbloco */

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

/* abre o arquivo */
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
        arquivo = fopen(filename, "rb");
        if (!arquivo) {
            fprintf(stderr, "Erro ao abrir o arquivo.\n");
            return -1;  
	    }
    }

	Superblock sb;
    /* o fread copia o q está no arquivo para sb */
    /* o ponteiro do arquivo tbm muda para depois do superbloco */
    /* verificamos se deu 1, pois tem q ser a mesma qtd de arquivos q eu li (1) */
    if (fread(&sb, sizeof(Superblock), 1, arquivo) != 1) {
		fprintf(stderr, "Erro ao ler o arquivo.\n");
        fclose(arquivo);
		return -1;
	} 
    
	/* vai com o ponteiro do início do arquivo até onde o diretório começa */
    fseek(arquivo, sb.offset, SEEK_SET); 
    /* agora a qtd de docs na livraria é a mesma q a do superbloco */
	lib->count = sb.qtd_doc;        
    
    /* alocando o espaço pra todos os docs q precisa ter na livraria */
	lib->docs = malloc(sizeof(Document)*sb.qtd_doc); /* podia ser *lib->count tbm mas acho q tanto faz */

	if (!lib->docs) {
		fprintf(stderr, "Erro ao alocar memoria.\n");
		fclose(arquivo); /* sempre lembrar de fechar!! */
        return -1;
	}

    int x = 0;

    if (fread(lib->docs, sizeof(Document), sb.qtd_doc, arquivo) != sb.qtd_doc) {
        fprintf(stderr, "Erro ao ler o arquivo.\n");
        free(lib->docs);
        x = -1;
    }

    fclose(arquivo);
    return x;
}

int gbv_find(const Library *lib, const char *procurado) {
    /* inicializando com -1 para o caso de n ter nome repetido */
    for (int i = 0; i < lib->count; i++) {
        if (strcmp(lib->docs[i].name, procurado) == 0) {
            return i;
        }
	}
    
    return -1;
}

/* adiciona novos arquivos à biblioteca */
int gbv_add(Library *lib, const char *archive, const char *docname) {
    if (!lib || !archive || !docname) {
        fprintf(stderr, "Ponteiro nulo.\n");
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

    /* vou até onde começa o diretório de destino (começo + offset) */
    fseek(destino, sb.offset, SEEK_SET); 
    /* agr vou até a posição final do arquivo q quero inserir para saber o tamanho dele */
    fseek(doc_insercao, 0, SEEK_END);
    /* esse vai ser o tamanho do documento */
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
    
    int idx = gbv_find(lib, docname); /* procurando índice onde docname está no vet de nomes  */
    
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
    lib->count++; /* agora temos mais elementos (ou a mesma qtd se tivermos feito substituição) */

	sb.qtd_doc = lib->count; /* atualizando biblioteca */
    sb.offset = ftell(destino); /* usando o fseek q fiz na linha 139 */
	
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

    if (idx == -1) { /* no caso de o elemento a ser excluído do arquivo n estar no arquivo */
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

    lib->count--;

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
	
    for (int i = 0; i < lib->count; i++) {
		format_date(lib->docs[i].date, data, sizeof(data));
        printf("Documento atual: %d -- ", i);
        printf("Nome: %s -- ", lib->docs[i].name);
        printf("Tamanho: %ld -- ", lib->docs[i].size);
        printf("Data de insercao: %s --", data);
        printf("Posicao (offset): %ld\n", lib->docs[i].offset);
		printf("--------------------------------------------------------------------------------------------");
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

    int bloco_atual = 0;
    char select; /* opção escolhida pelo usuário (p,q,n)*/
    unsigned char buffer[BUFFER_SIZE + 1];

    int qtd_blocos = (lib->docs[idx].size + BUFFER_SIZE - 1)/BUFFER_SIZE;

    while (1) {

        /* posição atual = inicio do documento + (bloco atual * tamanho dele) */
        long posicao = lib->docs[idx].offset + (bloco_atual * BUFFER_SIZE);
        fseek(arquivo, posicao, SEEK_SET); /* coloca o ponteiro do arquivo na posicao atual */

        int falta_ler = BUFFER_SIZE; 
        int ja_lido = bloco_atual*BUFFER_SIZE;
		
		if (ja_lido + BUFFER_SIZE > lib->docs[idx].size) {
            falta_ler = lib->docs[idx].size - ja_lido;
        }
		
        int lidos = fread(buffer, 1, falta_ler, arquivo);
        if (lidos < 0) {
            fprintf(stderr, "Erro ao ler o arquivo.\n");
            break;
        }
		
        buffer[lidos] = '\0';

        printf("\n--- Visualizando: %s (Bloco %d de %d) ---\n", docname, bloco_atual + 1, qtd_blocos);
        printf("%s\n", buffer);
        printf("-------------------------------------------\n");

		/* ve se já n ta no ultimo bloco pq senão n vou mostrar a opção de ir pro próximo */
		if (bloco_atual < qtd_blocos - 1)
        	printf("(n) -> proximo bloco\n");
        
		/* so vou dar a opção visual de ir pro ultimo bloco se eu ja n estiver no primeiro */
		if (bloco_atual > 0)
			printf("(p) -> bloco anterior\n");
        printf("(q) -> sair da visualizacao\n");
        
        scanf(" %c", &select);

        if (select == 'q')
            break;
        if (select == 'n') {
			if (bloco_atual < qtd_blocos - 1)
            	bloco_atual++;
			else
				printf("\nEste ja eh o ultimo documento.\n");
		}
			
        if (select == 'p') {
			if (bloco_atual > 0)
            	bloco_atual--;
			else
				printf("\nEste ja eh o primeiro documento.\n");
        if (select != 'p' && select != 'n' && select != 'q')
            printf("\nEntrada invalida.\n");
    }

    fclose(arquivo);
    return 0;
}
