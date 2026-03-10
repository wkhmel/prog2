#include <stdio.h>

struct maq {
	int prob;
	int id;
	struct maq *prox;
	struct maq *ant;
}

/*
struct lista_maq {
	int qtd;
	struct lista_maq *ini;
	struct lista_maq *fim;	
} */

struct lista (
	struct maq *sentinela;
	int id_prox;
}

struct lista *cria_lista() {
	lista *li = malloc(sizeof(struct lista));
	if (!li)
		return NULL;
	li->sentinela = malloc(sizeof(struct maq));
	if (!li->sentinela)
		return NULL;
       	li->sentinela->prox = li->sentinela;
	li->sentinela->ant = li->sentinela;	
	li->id_prox = 1;
	return li;
}

/* MINHA VERSAO ERRADA
struct lista_maq *cria_lista() {
	struct lista_maq *li = malloc(sizeof(lista_maq);
	if (!li)
		return NULL;
	li->qtd = 0;
	li->ini = li->fim = NULL;
       	return li;	
} */

int insere_ma(struct lista_maq *li, struct maq *ma) {
	struct maq *ma = malloc(sizeof(struct maq));
	if (!ma)
		return 0;

	ma->prob = rand()%101;
	ma->id = id;
	ma->prox = NULL;
	ma->ant = NULL;
	
	if (li->qtd == 0)
		li->ini = li->fim = ma;
	struct maq *aux = li->ini;
	else {
		while ((aux->prox != li->ini) && (ma->prob > aux->prob )) {
			
			
			
		}
				

	}
	cjto->qtd++;
	return 1;
}

