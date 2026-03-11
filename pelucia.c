#include <stdio.h>
#include <stdlib.h>

/* struct para a máquina */
struct ma {
    int prob;
    int id;
    struct ma *ant;
    struct ma *prox;
};

/* struct para a lista circular duplamente encadeada que contém as máquinas */
struct lista_ma {
    struct ma *sentinela;
    int qtd;
};

/* struct que cria uma lista vazia, colocando uma sentinela sem valores */
struct lista_ma *cria_lista () {
    struct lista_ma *li = malloc(sizeof(struct lista_ma));
    if (!li)
        return NULL;
    li->sentinela = malloc(sizeof(struct ma));
    if (!li->sentinela)
        return NULL;
    struct ma *s = li->sentinela;
    s->ant = s;
    s->prox = s;
    li->qtd = 0;
    return li;
}

/* função que insere novas máquinas dentro da lista */
int insere_li(struct lista_ma *li, int id) {
    struct ma *ma = malloc(sizeof(struct ma));
    if (!ma)
        return 0;
    ma->prob = rand() % 101;
    ma->id = id;
    struct ma *aux = li->sentinela->prox;
    while ((aux != li->sentinela) && (ma->prob < aux->prob))
        aux = aux->prox;
    ma->prox = aux;
    ma->ant = aux->ant;
    aux->ant->prox = ma;
    aux->ant = ma;
    li->qtd++;
    return 1;
}

int main() {
    printf("quantas maquinas voce quer criar?");
    int n;
    scanf("%d", &n);
    struct lista_ma *li = cria_lista();
    for (int i = 0; i < n; i++)
        insere_li(li, i);

}
