#include <stdlib.h>
#include <stdio.h>
#include "Cenario.h"

struct cenario *criar_cenario(ALLEGRO_BITMAP *imagem, int largura_mapa) {   
	if (!imagem) return NULL;
	
	struct cenario *c = (struct cenario*)malloc(sizeof(struct cenario));
        
	if (!c) return NULL;
	
	c->background = imagem;
	c->camera_x = 0;
	c->largura_mapa = largura_mapa;

	return c;
}

struct chao *criar_chao(int altura, ALLEGRO_BITMAP *imagem) {
	if (!imagem) return NULL;
	
	struct chao *g = (struct chao*)malloc(sizeof(struct chao));

	if (!g) return NULL;

	g->altura = altura;
	g->imagem = imagem;

	return g;
}

void desenhar_cenario(struct cenario *c, int altura_tela) {
	if (!c || !c->background) return;

	// preenche com mais do bitmap vertical caso ele não tenha o tamanho completo da minha tela
	int bg_h = al_get_bitmap_height(c->background);
	for (int y = 0; y < altura_tela; y = y + bg_h) {
        	al_draw_bitmap(c->background, -c->camera_x, y, 0);
	}
        
}

void desenhar_chao(struct chao *g, int camera_x) {
	if (!g || !g->imagem) return;

	int y = Y_SCREEN - g->altura;

	int largura_imagem = al_get_bitmap_width(g->imagem);

	// logica: n desenhar 3000 pixels de chão, desenhar só a parte visível
	int inicio = (camera_x / largura_imagem) * largura_imagem;
	int fim = camera_x + X_SCREEN + largura_imagem;

	for (int i = inicio; i < fim; i = i + largura_imagem) {
		al_draw_bitmap(g->imagem, i - camera_x, y, 0);
	}

}

void atualizar_cenario(struct cenario *c, struct player *omori, int largura_tela) {
	if (!c || !omori) return;

	c->camera_x = omori->x - (largura_tela)/2; //camera agora focaliza o jogadorzinho
        
	if (c->camera_x < 0) { // nao deixa a camera mostrar a parte vazia
        	c->camera_x = 0;
	}

	int limiteMaximo = c->largura_mapa - largura_tela; // isso impede que a camera vá além do fim do mapa
	if (limiteMaximo < 0) limiteMaximo = 0;
        
	if (c->camera_x > limiteMaximo) {
        	c->camera_x = limiteMaximo;
	}
}

void destruir_cenario(struct cenario *c) {
	if (!c) return;

	free(c);
}

void destruir_chao(struct chao *g) {
	if (!g) return;
	
	free(g);
}
