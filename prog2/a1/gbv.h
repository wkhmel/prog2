#ifndef GBV_H
#define GBV_H

#include <time.h>

#define MAX_NAME 256
#define BUFFER_SIZE 512   /*tamanho fixo do buffer em bytes*/

// Estrutura de metadados de cada documento
typedef struct {
    char name[MAX_NAME];   /*nome do documento*/
    long size;             // tamanho em bytes
    time_t date;           // data de inserção
    long offset;           // posição no container
} Document;

// Estrutura que representa a biblioteca (diretório em memória)
typedef struct {
    Document *docs;        // vetor dinâmico de documentos
    int count;             // número de documentos
} Library;

/* cria um arquivo container e coloca o espaço necessário de um superbloco p inicializar */
/* entrada: nome do arquivo container. saída: 0 em sucesso e -1 em erro */
int gbv_create(const char *filename);

/* abre um arquivo container e carrega seus dados na memoria */
/* entrada: ponteiro pra biblioteca e nome do arquivo container. saída: 0 em sucesso e -1 em erro */
int gbv_open(Library *lib, const char *filename);

/* busca um doc na biblioteca*/
/* entrada: ponteiro pra biblioteca e nome do documento. saída: índice do documento na biblioteca ou -1 se não encontrado */
int gbv_find(const Library *lib, const char *procurado);

/* adiciona um documento novo na blbioteca */
/* entrada: ponteiro para a biblioteca, nome do arquivo container e nome do documento a ser inserido. saída: 0 em sucesso e -1 em erro */
int gbv_add(Library *lib, const char *archive, const char *docname);

/* remove um documento da biblioteca e tbm do arquivo container */
/* entrada: ponteiro para a biblioteca, nome do arquivo container e nome do documento a ser removido. saída: 0 em sucesso e -1 em erro */
int gbv_remove(Library *lib, const char *archive, const char *docname);

/* lista todos os documentos da biblioteca (e tamanho, data, nome) */
/* entrada: ponteiro pra biblioteca. saída: 0 em sucesso e -1 em erro */
int gbv_list(const Library *lib);

/* exibe as informações de um documento (o que tem dentro dele em si)*/
/* entrada: ponteiro pra biblioteca, nome do arquivo container e nome do documento. saída: 0 em sucesso e -1 em erro */
int gbv_view(const Library *lib, const char *archive, const char *docname);

/* função pra comparar os documentos em ordem alfabetica*/
/* recebe um ponteiro para dois documentos (genéricos) e retorna um valor q representa a ordem (positivo, negativo ou zero) */
int ordem_alfabetica(const void *a, const void *b);

/* função q compara os docs em ordem de tamanho (menor pra maior)*/
/* recebe um ponteiro para dois documentos (genéricos) e retorna um valor na mesma lógica da função anterior */
int ordem_tamanho(const void *a, const void *b);

/* função q compara os docs em ordem cronológica (mais antigo pra mais novo)*/
/* recebe um ponteiro para dois documentos (genéricos) e retorna um valor na mesma lógica da função anterior */
int ordem_cronologica(const void *a, const void *b);

/* ordena os documentos da biblioteca com base em um critério, que pode ser ordem alfabética, tamanho ou ordem cronológica */
/* entrada: ponteiro para a biblioteca, nome do arquivo container e critério de ordenação. saída: 0 em sucesso e -1 em erro */
int gbv_order(Library *lib, const char *archive, const char *criteria);

#endif
