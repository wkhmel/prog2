#ifndef __OMORI_H__
#define __OMORI_H__

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Joystick.h"

#define VIDA_MAXIMA 5 // define o hp do personagem ao ser criado
#define VELOCIDADE_HORIZONTAL 4.0
#define IMPULSO_PULO -15.0
#define GRAVIDADE 1.0 // força da gravidade no mundo do jogo
#define FORCED_FALL 2.0 // a velocidade da queda fica mais rápida qnd forço o botão pra baixo durante a queda
#define DIST_SOLO 105 // altura do chao em pixels a partir da base
#define PIXELS_TRANSPARENTES_CHAO 13

// todas as poses que o omori pode fazer (dependendo da ação)
enum poses {
        PARADO,
        ANDANDO1,
        ANDANDO2,
        ANDANDO3,
        AGACHADO,        
        PULANDO,
        
        NUM_POSES
};

struct player {
        int altura, largura; // dimensões
        int hitbox_x, hitbox_y, hitbox_agachado_x, hitbox_agachado_y; // área de colisão
        int hitbox_atual_x, hitbox_atual_y; // hitbox muda dependendo de omori estar agachado ou em pé  
        int hp; //vida        
        int face; //rosto virado pra alguma direção (esquerda ou direita apenas)
        int x, y; // coord

        // velocidades horizontal e vertical para sistema de gravidade
        float vel_x, vel_y;
        float multiplicador_velocidade; // determina em quanto a velocidade padrão do omori será multiplicada (p/ bônus de speedup)
        int quadros_speedup; // contador para o tempo de duração de efeito de speedup de bônus
        
        // diz se está no chão ou nn
        bool NoChao;

        // impedir que tome muitos danos por segundo
        bool EhInvencivel;
        int PeriodoInvencivel;

        ALLEGRO_BITMAP *sprites[NUM_POSES]; //vetor de imagens
        ALLEGRO_BITMAP *hearts[2*VIDA_MAXIMA]; //vetor de imagens da vida do personagem

        enum poses emotes; //decide qual imagem vai ser usada 

        joystick *control; // controle do jogador
};

// entrada: altura, largura, face (esquerda ou direita), sistema de hitbox, posição x, posição y, velocidade_x, velocidade_y, valores máximos da posição de x e da posição de y
// saída: personagem criado, inicialmente no chão e parado
struct player *player_create(int face, int hitbox_x, int hitbox_y, int hitbox_agachado_x, int hitbox_agachado_y, int x, int y, float vel_x, float vel_y, int max_x, int max_y, ALLEGRO_BITMAP *sprites[NUM_POSES], ALLEGRO_BITMAP *hearts[2*VIDA_MAXIMA]);

// entrada: personagem e tamanho horizontal da câmera
// saída: void (desenha o personagem e os corações)
void player_draw(struct player *omori, int camera_x);

// entrada: personagem e posições máximas para cada eixo
// saída: void (atualiza a posição e movimenta o omori)
void player_atualizaPosicao(struct player *omori, int max_x, int max_y);

// entrada: personagem
// saída: void (atualiza a "pose"/frame do personagem de acordo com as movimentações atuais selecionadas)
void player_atualizaPose(struct player *omori);

// entrada: personagem
// saída: void (destrói esse personagem e todas as estruturas inerentes a ele: imagens, controle etc.)
void player_destroy(struct player *omori);

#endif
