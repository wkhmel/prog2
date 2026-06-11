#ifndef __OBSTACULOS_H__
#define __OBSTACULOS_H__

#include "Omori.h"
#define QTD_OBSTACULOS 6 

struct obstaculo {
        int x, y; //posições
        int hitbox_x, hitbox_y; //área de colisão

        //cada obstáculo vai ter sua própria imagem
        ALLEGRO_BITMAP *img_obstaculo;
        int altura, largura; // dimensões (determinadas pela img)

        int x_inicio; //onde começa (pra determinar colisão)
        int movimento_max; // máximo de movimento que um obstáculo pode fazer (se for móvel)
        float vel_x; // velocidade (para os móveis)

        int dano; // quanto esse obstáculo específico tira de vida

        struct obstaculo *prox; // lista encadeada
};

// recebe: lista de obstáculos atual (pode ser nula/vazia), posições x e y, altura e largura, áreas de colisão, imagem, limite de movimento horizontal, velocidade e dano causado
// retorna: o ponteiro para obstáculo. função: criar obstáculo
struct obstaculo* criar_obstaculo(struct obstaculo *lista_obstaculos, int x, int y, int hitbox_x, int hitbox_y, ALLEGRO_BITMAP *imagem, int limite, float vel_x, int dano);

// recebe: lista de obstáculos atual
// retorna: void. função: atualizar os obstáculos
void atualizar_obstaculo(struct obstaculo *lista_obstaculos);

// recebe: lista de obst. atual, jogador e ponteiro que indica se teve dano ou não;
// retorna: 1 se omori tomou dano ou 0 c.c. 
//função: verificar se teve dano, retirar vida do omori e criar uma pequena barreira pra dano excessivo 
int checar_dano_obstaculos(struct obstaculo *lista_obstaculos, struct player *omori);

// recebe: lista de obst. atual e componente horizontal da câmera;
// retorna: void. função: desenhar os obstáculos no mundo
void desenhar_obstaculo(struct obstaculo *lista_obstaculos, int camera_x);

// recebe: lista de obst. atual
// retorna: void. função: destruir os obstáculos (menos a img que é destruída em jogo.c)
struct obstaculo* destruir_obstaculos(struct obstaculo *lista_obstaculos);

#endif

