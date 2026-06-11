#include "Omori.h"

struct player *player_create(int face, int hitbox_x, int hitbox_y, int hitbox_agachado_x, int hitbox_agachado_y, int x, int y, float vel_x, float vel_y, int max_x, int max_y, ALLEGRO_BITMAP *sprites[NUM_POSES], ALLEGRO_BITMAP *hearts[2*VIDA_MAXIMA]) {        
	//verifica se tem faces válidas
	if (face > 1 || face < 0) {
		fprintf(stderr, "Erro: nao e possivel criar personagem com a face escolhida.\n");
		return NULL;
	}


	//aloca memória na heap para um novo personagem
	struct player *omori = (struct player*) malloc(sizeof(struct player));
	       
	if (!omori) {
		fprintf(stderr, "Erro: nao foi possivel criar o jogador.\n");
		return NULL;														
	}

	omori->face = face;
	omori->x = x;	
	omori->y = y;		
	
	//a princípio omori está em pé. hitbox muda quando ele estiver agachado.
	omori->hitbox_atual_x = omori->hitbox_x = hitbox_x; 
	omori->hitbox_atual_y = omori->hitbox_y = hitbox_y;
	omori->hitbox_agachado_x = hitbox_agachado_x;
	omori->hitbox_agachado_y = hitbox_agachado_y;
	
	omori->hp = VIDA_MAXIMA; // 5 vidas (corações)
	omori->emotes = PARADO; // começa parado
	
	//pega as imagens de fora (Jogo.c)
	for (int i = 0; i < NUM_POSES; i++) {
		omori->sprites[i] = sprites[i];
	}

	for (int i = 0; i < 2*VIDA_MAXIMA; i++) {
		omori->hearts[i] = hearts[i];
	}

	//altura e largura do personagem
	omori->altura = al_get_bitmap_height(omori->sprites[0]);										
	omori->largura = al_get_bitmap_width(omori->sprites[0]);	
	
	//verifica se posição inicial é válida; caso não seja, retorna NULL 
	if ((x - omori->hitbox_atual_x/2 < 0) || (x + omori->hitbox_atual_x/2 > max_x) || (y - omori->hitbox_atual_y/2 < 0) || (y + omori->hitbox_atual_y/2 > max_y)) {
		fprintf(stderr, "Erro: posicoes iniciais invalidas.\n");
		free(omori);
		return NULL;
	}
	
	//velocidades horizontal e vertical para sistema de gravidade
	omori->vel_x = vel_x;                                                                                
	omori->vel_y = vel_y;    
	omori->multiplicador_velocidade = 1.0;                                                                                     
	omori->quadros_speedup = 0; //começa sem bônus

	// começa no chão e passível de tomar dano
	omori->NoChao = true;
	omori->EhInvencivel = false;
	omori->PeriodoInvencivel = 0;
	
	//insere o controle
	omori->control = joystick_create();

	if (!omori->control) {
		fprintf(stderr, "Erro: nao foi possivel inicializar o controle do jogador.\n");
		free(omori);
		return NULL;
	}

	return omori;//retorna o novo personagem (o nome dele é omori)
}

void player_draw(struct player *omori, int camera_x) {
	if (!omori) return;

	int ladoFace = 0;
	if (omori->face == 0) { // 0 significa esquerda
		ladoFace = ALLEGRO_FLIP_HORIZONTAL; // muda o desenho pro outro lado
	}

	al_draw_bitmap(omori->sprites[omori->emotes], omori->x - camera_x - omori->largura/2, omori->y - omori->altura/2, ladoFace);

	// desenha coração quebrado ou cheio dependendo do hp do omori
	for (int i = 0; i < VIDA_MAXIMA; i++) {
		if (i < omori->hp)
			al_draw_bitmap(omori->hearts[i], 10 + (i * 30), 10, 0);
		else
			al_draw_bitmap(omori->hearts[i + VIDA_MAXIMA], 10 + (i * 30), 10, 0);
	}
}


void player_atualizaPosicao(struct player *omori, int max_x, int max_y) {
	if (!omori) return;

	if (omori->quadros_speedup > 0) {
		omori->quadros_speedup--;
		// volta à velocidade normal quando esse contador ir para zero
		if (!omori->quadros_speedup) omori->multiplicador_velocidade = 1.0;
	}

	omori->NoChao = false;

	// velocidade negativa porque vai para a esquerda
	if (omori->control->left && !omori->control->right) {
		omori->vel_x = -VELOCIDADE_HORIZONTAL*(omori->multiplicador_velocidade);
	} 
	else if (omori->control->right && !omori->control->left) {
		omori->vel_x = VELOCIDADE_HORIZONTAL*(omori->multiplicador_velocidade);
	} 
	else { 
		omori->vel_x = 0; //se não vai pra nenhum lado
	}

	
	// continua podendo se mexer horizontalmente, o pulo só muda o componente x
	if (omori->control->jump && omori->vel_y == 0) {
		omori->vel_y = IMPULSO_PULO;
		omori->control->jump = false; // evitar pulo duplo
	} 
	else if (omori->control->down && omori->vel_y > 0) {
		omori->vel_y = omori->vel_y + FORCED_FALL; 
	} // cai mais rapidinho se eu clico para ir pra baixo enquanto está no ar

	int solo = max_y - DIST_SOLO + PIXELS_TRANSPARENTES_CHAO;
	if (omori->vel_y != 0 || (omori->y + omori->hitbox_atual_y / 2 < solo)) {
		omori->vel_y = omori->vel_y + GRAVIDADE; // vai caindo com a gravidade
	}

	// posição recebe a velocidade
	omori->x = omori->x + omori->vel_x;
	omori->y = omori->y + omori->vel_y;

	//controle de limites do personagem (não sair dos limites da tela)
	if (omori->x - omori->hitbox_atual_x/2 < 0) {
		omori->x = omori->hitbox_atual_x/2;
	}
	if (omori->x + omori->hitbox_atual_x/2 > max_x) {
		omori->x = max_x - omori->hitbox_atual_x/2;
	}

	if (omori->y + omori->hitbox_atual_y/2 >= solo) {
		omori->y = solo - omori->hitbox_atual_y/2;
		omori->vel_y = 0;
		omori->NoChao = true;
	}

	if (omori->y - omori->hitbox_atual_y/2 < 0) {
		omori->y = omori->hitbox_atual_y/2;
		omori->vel_y = 0;
	}
}

void player_atualizaPose(struct player *omori) {
	if (!omori) return;

	// cálculo de diferença de altura entre o omori em pé e ele agachado
	int diff_altura = omori->hitbox_y - omori->hitbox_agachado_y;

	// se n está no chão, está pulando/caindo
	if (!omori->NoChao) {
		if (omori->emotes == AGACHADO) {
			// caso ele começou a pular logo dps de estar agachado, reajusta o y
			omori->y = omori->y - (diff_altura / 2);
		}

		omori->emotes = PULANDO;
		// hitbox dele continua sendo a dele em pé
		omori->hitbox_atual_x = omori->hitbox_x;
		omori->hitbox_atual_y = omori->hitbox_y;
	} else {
		if (omori->control->left || omori->control->right) {
			if (omori->emotes == AGACHADO) {
				// outro reajuste do y pra qnd ele estava agachado e começa a andar pros lados
				// impede q o omori afunde no chão
				omori->y = omori->y - (diff_altura / 2);
			}

			// alterna entre 3 sprites do personagem caminhando
			static int alternador = 0;
			alternador++;
                        
			if (alternador % 24 < 8) {
				omori->emotes = ANDANDO1;
			}
			else if (alternador % 24 < 16) {  
				omori->emotes = ANDANDO2;
			}
			else omori->emotes = ANDANDO3;

			//a hitbox dele é a mesma de ele em pé
			omori->hitbox_atual_x = omori->hitbox_x;
			omori->hitbox_atual_y = omori->hitbox_y;
		} 
		else if (omori->control->down) {
			// se ele n estava agachado antes e agora vai agachar
			if (omori->emotes != AGACHADO) {
				// empurra o centro de y p baixo p ele continuar com os pés no chão
				omori->y = omori->y + (diff_altura / 2);
			}
			omori->emotes = AGACHADO;
			//hitbox difere quando ele está agachado (largura maior e altura menor)
			omori->hitbox_atual_x = omori->hitbox_agachado_x;
			omori->hitbox_atual_y = omori->hitbox_agachado_y;
                }
		else {
			if (omori->emotes == AGACHADO) {
				// se ele estava agachado e agr vai ficar parado, puxa y pra cima
				omori->y = omori->y - (diff_altura / 2);
			}
			omori->emotes = PARADO; // n ta indo pra nenhum lado
			omori->hitbox_atual_x = omori->hitbox_x;
			omori->hitbox_atual_y = omori->hitbox_y;
		}
	}

	//qual lado vira
	if (omori->control->left) omori->face = 0;
	if (omori->control->right) omori->face = 1;

}


void player_destroy(struct player *omori){														                                                            
	if (!omori) return;

	joystick_destroy(omori->control);//destrói o controle do personagem

	free(omori); //libera a memória do personagem 
}

