#include "Plataformas.h"

struct plataforma *criar_plataforma(struct plataforma *lista_plataformas, int x, int y, ALLEGRO_BITMAP *imagem, int hitbox_y, float vel_x, int limite) {
	struct plataforma *p = malloc(sizeof(struct plataforma));

	if (!p) return lista_plataformas;

	p->x = x;
	p->y = y;
	p->x_inicio = x; // primeira posição dele no mapa
	
	p->imagem = imagem;
	p->hitbox_y = hitbox_y;
	p->altura = al_get_bitmap_height(imagem);
	p->largura = al_get_bitmap_width(imagem);

	p->vel_x = vel_x;
	p->movimento_max = limite;
	p->prox = lista_plataformas; // vai no começo da listinha

	return p;
}

void desenhar_plataforma(struct plataforma *lista_plataformas, int camera_x) {
	if (!lista_plataformas) return;

	struct plataforma *aux = lista_plataformas;

	while (aux) {
		al_draw_bitmap(aux->imagem, aux->x - camera_x, aux->y, 0);
		aux = aux->prox;
	}
}

void atualizar_plataforma(struct plataforma *lista_plataformas) {
	if (!lista_plataformas) return;

	struct plataforma *aux = lista_plataformas;

	while (aux) {
	// só executa se for móvel (velocidade difere de zero)
		if (aux->vel_x != 0) {
			aux->x = aux->x + aux->vel_x;

			if (aux->x > aux->x_inicio + aux->movimento_max || aux->x < aux->x_inicio - aux->movimento_max) {
				aux->vel_x = -aux->vel_x;
			}
		}
		aux = aux->prox;
	}
}

int checar_plataforma(struct player *omori, struct plataforma *lista_plataformas) {
	if (!omori || !lista_plataformas) return 0;

	struct plataforma *aux = lista_plataformas;
	int pisou = 0;

	int piso = omori->y + omori->hitbox_atual_y / 2;
	int piso_antes = piso - omori->vel_y;

	int esquerda = omori->x - omori->hitbox_atual_x/2;
	int direita  = omori->x + omori->hitbox_atual_x/2;

	while (aux) {
		//coords bloquinho
		int topo_p = aux->y + (aux->altura - aux->hitbox_y);
		int esq_p = aux->x;
		int dir_p = aux->x + aux->largura;

		bool sobreposicao = false;
		// verifica se está em sobreposição com a plataforma
		if (direita > esq_p && esquerda < dir_p) sobreposicao = true;
		
		bool descendo = false;
		if (omori->vel_y >= 0) descendo = true; // se está no ar especificamente descendo

		bool encostouTopo = false;
		if ((piso >= topo_p) && ((piso_antes) <= topo_p)) encostouTopo = true;

		// identifica a plataforma como chão quando omori desce, cai em cima dela e fica sobre ela
		if (sobreposicao && descendo && encostouTopo) {
			if (omori->control->down) {
				aux = aux->prox;
				continue; // pula pra próxima plataforma (mecânica de cair ao agachar)
			}

			//pousa normalmente se o usuário n apertar pra ir pra baixo
			omori->y = topo_p - omori->hitbox_atual_y/2;
			omori->vel_y = 0;
			omori->NoChao = true;
			omori->x = omori->x + aux->vel_x; // move omori junto com a plataforma
			pisou = 1;
			break; //já está em uma plataforma válida 
		}
		aux = aux->prox;
	}

	return pisou;
}

struct plataforma *destruir_plataforma(struct plataforma *lista_plataformas) {
	if (!lista_plataformas) return NULL;
    	
	struct plataforma *aux = lista_plataformas;

	while (aux) {
		struct plataforma *prox = aux->prox;
		free(aux);
		aux = prox;
	}

	return NULL;
}
