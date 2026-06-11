#ifndef __CENARIO_H__
#define __CENARIO_H__

#include "Omori.h"
#define X_SCREEN 520
#define Y_SCREEN 520
#define LARGURA_MAPA 3000

struct cenario {
        ALLEGRO_BITMAP *background;
        int camera_x;
        int largura_mapa;
};

struct chao {
    int altura;
    ALLEGRO_BITMAP *imagem;
};

// entrada: imagem de cenário e largura total do mapa
// saída: cenário criado
struct cenario *criar_cenario(ALLEGRO_BITMAP *imagem, int largura_mapa);

// entrada: altura do chão a ser criado e imagem desse chão
// saída: chão criado
struct chao *criar_chao(int altura, ALLEGRO_BITMAP *imagem);

// entrada: cenário e altura da tela
// saída: void (desenha o cenário)
void desenhar_cenario(struct cenario *c, int altura_tela);

// entrada: chão e posição horizontal da câmera
// saída: void (chão desenhado)
void desenhar_chao(struct chao *g, int camera_x);

// entrada: cenário, jogador e largura total da tela (menor q o mapa)
// saída: void (atualiza o cenário, uma vez que ele se movimenta conforme a câmera fixa no personagem)
void atualizar_cenario(struct cenario *c, struct player *omori, int largura_tela);

// entrada: cenário
// saída: void (destrói todo o cenário)
void destruir_cenario(struct cenario *c);

// entrada: chão
// saída: void ( destrói o chão)
void destruir_chao(struct chao *g);

#endif
