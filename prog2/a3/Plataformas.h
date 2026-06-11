#ifndef __PLATAFORMAS_H__
#define __PLATAFORMAS_H__

#include "Omori.h"
#include "Cenario.h"
#define QTD_PLATAFORMAS 3

struct plataforma { 
    int x, y; //posições no mapa

    ALLEGRO_BITMAP *imagem;
    int largura, altura;
	int hitbox_y; //verdadeira área de colisão vertical

    float vel_x; //velocidade da plataforma (se for móvel)

    int x_inicio;
    int movimento_max;

	struct plataforma *prox;
};

// entrada: posições da plataforma, imagem para ela, hitbox vertical, sua velocidade e movimento horizontal máximo (se for móvel)
// saída: plataforma criada
struct plataforma *criar_plataforma(struct plataforma *lista_plataformas, int x, int y, ALLEGRO_BITMAP *imagem, int hitbox_y, float vel_x, int limite);

// entrada: plataforma e posição horizontal da câmera
// saída: void (plataforma é desenhada)
void desenhar_plataforma(struct plataforma *lista_plataformas, int camera_x);

// entrada: plataforma
// saída: plataforma com movimento atualizado (caso seja móvel)
void atualizar_plataforma(struct plataforma *lista_plataformas);

// entrada: jogador e plataforma
// saída: 0 se n subiu em uma plataforma e 1 se sim (checa se tem plataforma e a colisao com ela)
int checar_plataforma(struct player *omori, struct plataforma *lista_plataformas);

// entrada: plataforma
// saída: plataforma nula (destrói a plataforma)
struct plataforma *destruir_plataforma(struct plataforma *lista_plataformas);

#endif
