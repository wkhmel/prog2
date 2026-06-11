#include "Bonus.h"

struct item_bonus *criar_item(struct item_bonus *lista, int x, int y, ALLEGRO_BITMAP *imagem, float speedup, int heal) {
	if (!imagem) return lista;
    
	struct item_bonus *novo = (struct item_bonus*)malloc(sizeof(struct item_bonus));
    
	if (!novo) return lista;

	novo->x = x;
	novo->y = y;

	novo->imagem = imagem;
	novo->altura = al_get_bitmap_height(imagem);
	novo->largura = al_get_bitmap_width(imagem);

	//bônus q podem ser proporcionados
	novo->speedup = speedup;
	novo->heal = heal;
	novo->coletado = false;

	novo->prox = lista;

	return novo;
}

void desenhar_itens(struct item_bonus *lista, int camera_x) {
	if (!lista) return;

	struct item_bonus *atual = lista;

	while (atual) {
		// apenas desenha se tiver img e se n tiver sido coletado ainda
		if (atual->imagem && !atual->coletado) {
			al_draw_bitmap(atual->imagem, atual->x - camera_x - atual->largura / 2, atual->y - atual->altura /2, 0);
		}
		atual = atual->prox;
	}
}

int checar_coleta_itens(struct item_bonus *lista, struct player *omori) {
	if (!omori || !lista) return 0;

	struct item_bonus *atual = lista;
	int foiColetado = 0; // retorno da minha função (0 p/ não coletado, 1 p/ sim)

	int omori_esq = omori->x - omori->hitbox_atual_x / 2;
    	int omori_dir = omori->x + omori->hitbox_atual_x / 2;
    	int omori_topo = omori->y - omori->hitbox_atual_y / 2;
    	int omori_base = omori->y + omori->hitbox_atual_y / 2;

	while (atual) {
		if (!atual->coletado) {
			int item_esq = atual->x - atual->largura / 2;
			int item_dir = atual->x + atual->largura / 2;
			int item_topo = atual->y - atual->altura / 2;
			int item_base = atual->y + atual->altura / 2;

			if (omori_esq < item_dir && omori_dir > item_esq && omori_topo < item_base && omori_base > item_topo) {
				atual->coletado = true; // n vai passar pelo al_draw_bitmap do desenhar_itens
				foiColetado = 1;
			
				if (atual->heal > 0) {
					omori->hp = omori->hp + atual->heal;
					if (omori->hp > VIDA_MAXIMA) omori->hp = VIDA_MAXIMA;
				}

				if (atual->speedup > 0) {
					omori->multiplicador_velocidade = 1 + atual->speedup; //multiplica a velocidade
					omori->quadros_speedup = 120; // duração do speedup: 4 segundos
				}
			}
		}
		atual = atual->prox;
	}

	return foiColetado;
}

struct item_bonus *destruir_itens(struct item_bonus *lista) {
	if (!lista) return NULL;

	struct item_bonus *atual = lista;

	while (atual) {
		struct item_bonus *prox = atual->prox;
		free(atual);
		atual = prox;
	}
	return NULL;
}
