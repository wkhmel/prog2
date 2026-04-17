#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gbv.h"
#include "util.h"

typedef struct {
        int qtd_doc; // quantos documentos esse superbloco tem
        long offset; // em qual offset começa a área do diretório
} Superblock;

/* cria um container e coloca o espaço necessário de um superbloco p/ inicializar */
/* retorna 1 em sucesso e 0 em erro */
int gbv_create(const char *filename) {
	FILE *arquivo = fopen(filename, "wb");
        
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return -1;
    }
        
    Superblock sb;
    sb.qtd_doc = 0; /* começa sem documentos */
    sb.offset = sizeof(Superblock); /* ele vem dps do espaço ocupado pelo superbloco */

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
		fprintf(stderr, "Ponteiro nulo\n");
		return -1;
	}

    FILE *arquivo = fopen(filename, "rb"); /* abrindo o documento */

    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return -1;
	}

	Superblock sb;
    /* o fread copia o q está no arquivo para sb */
    /* o ponteiro do arquivo tbm muda para depois do superbloco */
    /* verificamos se deu 1, pois é o resultado que fread dá em sucesso */
    if (fread(&sb, sizeof(Superblock), 1, arquivo) != 1) {
		fprintf(stderr, "Erro ao ler o arquivo.\n");
        fclose(arquivo);
		return -1;
	} 
    
	/* vai c o ponteiro do início do arquivo até onde o diretório começa */
    fseek(arquivo, sb.offset, SEEK_SET); 
    /* agora a qtd de docs na livraria é a mesma q a do superbloco */
	lib->count = sb.qtd_doc;        
    
    /* alocando o espaço pra todos os docs q precisa ter na livraria */
	lib->docs = malloc(sizeof(Document)*sb.qtd_doc);

	if (!lib->docs) {
		fprintf(stderr, "Erro ao alocar memoria.\n");
		fclose(arquivo);
        return -1;
	}

    int x = 0;

    if (fread(lib->docs, sizeof(Document), sb.qtd_doc, arquivo) != sb.qtd_doc) {
        fprintf(stderr, "Erro ao ler o arquivo.\n");
        free(lib->docs);
        x = -1;
    }

    fclose(arquivo);
    /* para não ter q fechar dentro e fora da verificação do malloc */
    return x;
}

int gbv_find(Library *lib, char *procurado) {
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
        fprintf(stderr, "Ponteiro nulo\n");
        return -1;
	}

	FILE *doc_insercao = fopen(docname, "rb"); /* abrindo arquivo q quero inserir */

    if (!doc_insercao) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return -1;
    }

    FILE *destino = fopen(archive, "rb+"); /* destino q vai receber a inserção */

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

    /* vou até onde começa o diretório de destino */
    fseek(destino, sb.offset, SEEK_SET); 
    /* agr vou até a posição final do arquivo q quero inserir para saber o tamanho dele */
    fseek(doc_insercao, 0, SEEK_END);
    /* esse vai ser o tamanho do documento */
    long tam_arquivo = ftell(doc_insercao);
    /* resetando para o começo */    
    rewind(doc_insercao);
    /* agora estamos na posição em q começa o diretório novo */
    fseek(destino, sb.offset, SEEK_SET);

    /* declaramos o buffer */
    unsigned char buffer[BUFFER_SIZE];
    size_t lidos; /* qtd de bytes lidos em cada fwrite */

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
        fprintf(stderr, "Falha ao criar documento.\n");
        fclose(doc_insercao);
        fclose(destino);
        return -1;
    }

    /* se deu certo o realloc, posso atualizar o lib->docs para o auxiliar q criei */
    lib->docs = aux;
    
    int idx = gbv_find(lib, docname);
    
    /* pela definição do trabalho, substituímos artigos mais antigos com o mesmo nome */
    if (idx != -1) {
        for (int i = idx; i < lib->count - 1; i++) {
            lib->docs[i] = lib->docs[i + 1];            
        }
        lib->count--;
    }

    /* passando todos os arquivos do docname para o último documento da biblioteca */
    strncpy(lib->docs[lib->count].name, docname, MAX_NAME - 1);
    lib->docs[lib->count].name[MAX_NAME - 1] = '\0';
    lib->docs[lib->count].size = tam_arquivo;
    lib->docs[lib->count].offset = sb.offset;
    lib->count++; /* agora temos mais elementos (ou a mesma qtd se tivermos feito substituição )*/

	sb.qtd_doc = lib->count;
    sb.offset = ftell(destino);
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

    int idx = gbv_find(lib, docname);

    if (idx == -1) { /* caso de o elemento a ser excluído do arquivo n estar no arquivo */
        fprintf(stderr, "Arquivo %s nao encontrado.\n", docname);
        return -1;   
    }

    for (int i = idx; i < lib->count - 1; i++) {
        lib->docs[i] = lib->docs[i + 1];
    }

    lib->count--;

   return 0;
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

    printf("Quantidade total de documentos: %d\n", lib->count);

    for (int i = 0; i < lib->count; i++) {
        printf("Documento atual: %d -- ", i);
        printf("Nome: %s -- ", lib->docs[i].name);
        printf("Tamanho: %ld -- ", lib->docs[i].size);
        printf("Data de insercao: %s --", format_date(lib->docs[i].date, data, sizeof(data)));
        printf("Posicao (offset): %ld\n", lib->docs[i].offset);
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
        printf("(n) -> proximo bloco\n");
        printf("(p) -> bloco anterior\n");
        printf("(q) -> sair da visualizacao\n");
        
        scanf(" %c", &select);

        if (select == 'q')
            break;
        if (select == 'n' && bloco_atual < qtd_blocos - 1)
            bloco_atual++;
        if (select == 'p' && bloco_atual > 0)
            bloco_atual--;
        if (select != 'p' && select != 'n' && select != 'q')
            printf("\nEntrada invalida.\n");
    }

    fclose(arquivo);
    return 0;
}

