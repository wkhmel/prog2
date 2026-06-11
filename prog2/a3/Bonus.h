#ifndef __BONUS_H__
#define __BONUS_H__

#include "Omori.h"
#define QTD_BONUS 2

struct item_bonus {
	int x, y; // posiçoes no mapa
	int altura, largura; //dimensões

	ALLEGRO_BITMAP *imagem;

	float speedup; //pode aumentar a velocidade em "speedup" vezes
	int heal;//pode dar cura

	bool coletado; // se ja foi pego ou nn
	struct item_bonus *prox; // lista encadeada de itens

	 // bool ehMagico (comentado pq talvez eu implemente depois)
};

// entrada: lista de itens de bônus até agora (pode estar vazia), posições onde se encontra, imagem, bônus de velocidade e bônus de cura (pode ter ou não)
// saída: item criado e inserido na lista.
struct item_bonus *criar_item(struct item_bonus *lista, int x, int y, ALLEGRO_BITMAP *imagem, float speedup, int heal);

// entrada: lista de itens de bônus e posiçao da camera
// saída: void (desenha os itens)
void desenhar_itens(struct item_bonus *lista, int camera_x);

// entrada: player e lista de itens de bônus
// saída: 1 se coletado e 0 c.c. (faz a lógica da coleta do item quando omori "colide" com esse item)
int checar_coleta_itens(struct item_bonus *lista, struct player *omori);

// entrada: lista de itens de bônus
// saída: null (destrói os elementos e libera essa lista)
struct item_bonus *destruir_itens(struct item_bonus *lista);

#endif
