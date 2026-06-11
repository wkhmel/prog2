#include "Obstaculos.h"

struct obstaculo* criar_obstaculo(struct obstaculo *lista_obstaculos, int x, int y, int hitbox_x, int hitbox_y, ALLEGRO_BITMAP *imagem, int limite, float vel_x, int dano) {
        if (!imagem) {
		fprintf(stderr, "Erro: imagem do obstaculo nao foi encontrada.\n");
		return lista_obstaculos;
	}
	
	struct obstaculo *novo = (struct obstaculo*)malloc(sizeof(struct obstaculo));

        if (!novo) return lista_obstaculos;

        novo->x_inicio = novo->x = x;
        novo->y = y;
        novo->hitbox_x = hitbox_x;
        novo->hitbox_y = hitbox_y;
        novo->img_obstaculo = imagem;

        novo->altura = al_get_bitmap_height(novo->img_obstaculo);
        novo->largura = al_get_bitmap_width(novo->img_obstaculo);
        
        novo->movimento_max = limite; //p obstáculos móveis
        novo->vel_x = vel_x;
        novo->dano = dano;
        //encadeia novo nodo com o começo da lista q já existe 
        novo->prox = lista_obstaculos;

        return novo;
} 

void atualizar_obstaculo(struct obstaculo *lista_obstaculos) {
        struct obstaculo *atual = lista_obstaculos;

        while (atual) {
                if (atual->vel_x != 0) {
                        atual->x = atual->x + atual->vel_x;

                        if (atual->x > atual->x_inicio + atual->movimento_max || atual->x < atual->x_inicio - atual->movimento_max) {
                                atual->vel_x = -atual->vel_x; // obstáculo fica patrulhando
                        }
                }
                atual = atual->prox;
        }
}


int checar_dano_obstaculos(struct obstaculo *lista_obstaculos, struct player *omori) {
        if (!omori || omori->hp <= 0) return 0;

        int tomouDano = 0; //vai ser o return. 0 para n tomar dano e 1 para tomar

        // se o Omori acabou de tomar dano, reduz o tempo de invencibilidade frame por frame
        // (lógica para o Omori não tomar todo o dano tudo de uma vez)
        if (omori->EhInvencivel) {
                omori->PeriodoInvencivel--;
                if (omori->PeriodoInvencivel <= 0) {
                        omori->EhInvencivel = false;//apto a receber dano novamente
                }
                return tomouDano;
        }

        struct obstaculo *atual = lista_obstaculos;

        while (atual != NULL) {
                // coords player
                int omori_esquerda = omori->x - omori->hitbox_atual_x / 2;
                int omori_direita  = omori->x + omori->hitbox_atual_x / 2;
                int omori_topo     = omori->y - omori->hitbox_atual_y / 2;
                int omori_base     = omori->y + omori->hitbox_atual_y / 2;

                if (omori_esquerda < atual->x + atual->hitbox_x &&
                    omori_direita > atual->x &&
                    omori_topo < atual->y + atual->hitbox_y &&
                    omori_base > atual->y) 
                {
			omori->hp = omori->hp - atual->dano; //reduz progressivamente a vida ao encostar
			if (omori->hp < 0) omori->hp = 0;
			omori->EhInvencivel = true;
			omori->PeriodoInvencivel = 30;
			tomouDano = 1;
			break;
		}
		atual = atual->prox;
	}
	
	return tomouDano;
}

void desenhar_obstaculo(struct obstaculo *lista, int camera_x) {
	struct obstaculo *atual = lista;
	while (atual) {
		if (atual->img_obstaculo) {
			int flip = 0;
			// inverte qnd o obstáculo está indo pra outro lado
			if (atual->vel_x < 0) flip = ALLEGRO_FLIP_HORIZONTAL;
			al_draw_scaled_bitmap(atual->img_obstaculo, 0, 0,
			al_get_bitmap_width(atual->img_obstaculo),
			al_get_bitmap_height(atual->img_obstaculo),
			atual->x - camera_x, atual->y,
			atual->largura, atual->altura, flip);
		}
		atual = atual->prox;
	}
}


struct obstaculo* destruir_obstaculos(struct obstaculo *lista_obstaculos) {
	if (!lista_obstaculos) return NULL;
 
	struct obstaculo *atual = lista_obstaculos;
	struct obstaculo *proximo;
        
	while (atual) {
		proximo = atual->prox;
		free(atual);
		atual = proximo;      
	}
	return NULL;
}

