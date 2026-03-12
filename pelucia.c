    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>

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
        if (!ma || !li)
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

    /* função que recebe uma máquina e a retira da lista */
    int remove_li(struct lista_ma *li, struct ma *ma) {
        struct ma *aux = li->sentinela->prox;
        while (aux != li->sentinela) {
            if (ma == aux) {
                aux->ant->prox = aux->prox;
                aux->prox->ant = aux->ant;
                free(aux);
                aux = NULL;
                li->qtd--;
                return 1;
            }
            aux = aux->prox;
        }
        return 0;
    }

    /* função responsável pela jogada */
    void jogada(struct lista_ma *li) {
        int jogada = rand() % 101;
        int num_ma = rand() % li->qtd;
        struct ma *aux = li->sentinela->prox;
        printf("Chance do jogador: %d\n", jogada);
        printf("Numero da maquina: %d\n", num_ma);
        for (int i = 0; i < num_ma; i++) /* não verifiquei se aux n volta pro começo pq vou supor que a máquina sorteada tem que estar lá de qualquer jeito */
            aux = aux->prox;
        printf("Chance da maquina: %d\n", aux->prob);
        if (jogada > aux->prob) 
            printf("Que pena! Jogador perdeu\n");
        else {
            printf("Parabens! Jogador ganhou a maquina\n");
            remove_li(li, aux);   
        }
    }

    void oqsobra(struct lista_ma *li) {
        printf("maquinas que restaram:\n");
        struct ma *aux = li->sentinela->prox;
        while (aux != li->sentinela) {
            printf("id da maquina: %d\n", aux->id);
            printf("probabilidade da maquina: %d\n", aux->prob);
            aux = aux->prox;
        }
    }

    int main() {
        srand(time(NULL));
        printf("Quantas maquinas serao criadas? ");
        int n;
        scanf("%d", &n);
        struct lista_ma *li = cria_lista();
        for (int i = 0; i < n; i++)
            insere_li(li, i);
        int select = -1;
        while (li->qtd > 0 && select != 0) {
            printf("Insira 1 para jogar e 0 para encerrar o dia\n");
            scanf("%d", &select);
            if (select == 1)
                jogada(li);
        }
        oqsobra(li);
    }
