#include <stdio.h>
#include <stdlib.h>

typedef struct {
        int qtd_doc; // quantos documentos esse superbloco tem
        long offset; // em qual offset começa a área do diretório
} Superbloco;

/* cria um container e coloca o espaço necessário de um superbloco p/ inicializar */
/* retorna 1 em sucesso e 0 em erro */
int gbv_create(const char *filename) {
        FILE *arquivo = fopen(filename, "wb");
        
        if (!arquivo) {
                fprintf(stderr, "Erro ao abrir o arquivo.\n");
                return 0;
        }

        int qtd_doc = 0; /* começa sem documentos */
        long offset = sizeof(int) + sizeof(long) /* ele vem dps do espaço do superbloco */

        fwrite(&qtd_doc, sizeof(int), 1, arquivo); /* recebe o endereço do valor que quero colocar no arquivo e a quantidade (1) */
        fwrite(&offset, sizeof(long), 1, arquivo);
        fclose(arquivo);

        return 1;
}

/* abre o arquivo */
int gbv_open(Library *lib, const char *filename) {
        FILE *arquivo = fopen(filename, "rb"); /* abrindo o documento */ 
        int qtd_doc;
        long offset_dir;

        if (!arquivo) {
                fprintf(stderr, "Erro ao abrir o arquivo.\n");
                return 0;
        }

        /* lendo, do arquivo, a quantidade e o tipo de dado do arquivo para armazenar no buffer */
        fread(&qtd_doc, sizeof(int), 1, arquivo);
        fread(&offset_dir, sizeof(long), 1, arquivo); 
        /* o offset está em posição absoluta, então lemos do começo */
        fseek(arquivo, offset, SEEK_SET); 
        lib->count = qtd_docs;

        if (lib->count <= 0)
                lib->docs = NULL;
        else {
                lib->docs = malloc(sizeof(Document)*qtd_doc);
                if (!lib->docs) {
                        fprintf(stderr, "Erro ao abrir o arquivo.\n");
                        fclose(arquivo);
                        return 0;
                }
        }

        lib->count = qtd_docs;
        fclose(arquivo);
        return 1;
}

/* adiciona novos arquivos à biblioteca */
int gbv_add(Library *lib, const char *archive, const char *docname) {
        FILE *doc_insercao = fopen(docname, "rb"); /* arquivo de origem q quero inserir */
        /* guarda as informações do comecinho do buffer */
        int qtd_atual;
        long offset_atual;
        
        if (!doc_insercao) {
                fprintf(stderr, "Erro ao abrir o arquivo.\n");
                return 0;
        }

        FILE *destino = fopen(archive, "rb+"); /* destino q vai receber a inserção */

        if (!destino) {
                fprintf(stderr, "Erro ao abrir o arquivo.\n");
                fclose(doc_insercao);
                return 0;
        }
        
        fread(&qtd_atual, sizeof(int), 1, destino);
        fread(&offset_atual, sizeof(long), 1, destino);
        
        /* vou até o início do diretório de destino */
        fseek(destino, offset_atual, SEEK_SET); 
        /* agr vou até a posição final do arquivo q quero inserir para saber o tamanho dele */
        fseek(doc_insercao, 0, SEEK_END);
        /* agr q estou na posição final do arquivo, esse vai ser o tamanho do documento */
        long tam_arquivo = ftell(doc_insercao);
        /* volto para o começo de arquivo a ser inserido, pq senão n teria nada para ler qnd eu tentasse */
        fseek(doc_insercao, 0, SEEK_SET);

        char buffer[BUFFER_SIZE];
        size_t lidos;

        while ((lidos = fread(buffer, 1, BUFFER_SIZE, doc_insercao)) > 0) {
                fwrite(buffer, 1, lidos, destino)
        }

        /* cria um vetor auxiliar que recebe realloc para aumentar o seu tamanho */
        /* não alteramos diretamente o lib->docs para não o perdermos caso a realocação não dê certo */
        Document *aux = realloc(lib->docs, sizeof(Document)*(lib->count + 1));

        if (!aux) {
                fprintf(stderr, "Falha ao criar documento.\n");
                return 0;
        }

        /* se o realloc deu certo, atualizo o lib->docs para o auxiliar que criei */
        lib->docs = aux;
        /* o tamanho do documento do último índice é o tamanho do arquivo */
        lib->docs[lib->count].size = tam_arquivo;
        /* mesma coisa para o offset: recebe o atual do destino */
        lib->docs[lib->count].offset = offset_atual;
        lib->count++;
}
