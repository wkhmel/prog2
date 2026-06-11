#ifndef __JOGO_H__
#define __JOGO_H__

#include "Cenario.h"
#include "Obstaculos.h"
#include "Plataformas.h"
#include "Bonus.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_ttf.h>

enum ESTADO_JOGO { MENU, JOGAR, VITORIA, DERROTA, QTD_ESTADOS };
enum OPCOES_MENU { INICIAR, SAIR };
enum SONS { MENU_SCROLL, MENU_SELECIONAR, SOM_DANO, SOM_PULO, SOM_BONUS, QTD_SONS };
enum CENARIOS { CENARIO_MENU, CENARIO_JOGANDO, CENARIO_VITORIA, CENARIO_DERROTA, QTD_CENARIOS };

struct jogo {
	// controlar jogo
	enum ESTADO_JOGO estado_atual;
	enum OPCOES_MENU OpcaoSelecionada;

	// personagens e entidades
	struct player *omori;
	struct cenario *cenarios[QTD_CENARIOS]; // vetor de cenarios para cada acontecimento
	struct obstaculo *inicio_obstaculo; //lista encadeada de obstáculos
	struct chao *chao;
	struct plataforma *inicio_plataforma;// lista encadeada de plataformas
	struct item_bonus *inicio_bonus; // lista encadeada de itens de bônus

	//alegro
	ALLEGRO_TIMER *timer;	
	ALLEGRO_EVENT_QUEUE *queue;	
	ALLEGRO_DISPLAY *display;
	ALLEGRO_FONT *font;
	ALLEGRO_FONT *titulo;
	ALLEGRO_AUDIO_STREAM *musicas[QTD_ESTADOS]; // músicas contínuas do jogo
	ALLEGRO_SAMPLE *sons[QTD_SONS]; // samples rápidos (barulho de pulo, enter na opção desejada etc.)

	// todas as imagens (corações, cenários, personagem, corações de vida, chão, obstáculos, plataformas, itens bônus)
	ALLEGRO_BITMAP *fullheart;
	ALLEGRO_BITMAP *brokenheart;
	ALLEGRO_BITMAP *img_hearts[2*VIDA_MAXIMA];
	ALLEGRO_BITMAP *img_cenarios[QTD_CENARIOS];
	ALLEGRO_BITMAP *img_sprites[NUM_POSES];
	ALLEGRO_BITMAP *img_chao;
	ALLEGRO_BITMAP *img_obstaculos[QTD_OBSTACULOS];
	ALLEGRO_BITMAP *img_plataformas[QTD_PLATAFORMAS];
	ALLEGRO_BITMAP *img_bonus[QTD_BONUS];

};

// cria o jogo, o cenário e os recursos do allegro
// retorna o ponteiro de struct jogo criado
struct jogo *criar_jogo();

// entrada: musica do allegro
// saída: void (toca essa musica)
void colocar_musica(ALLEGRO_AUDIO_STREAM *musica);

// entrada: jogo e inteiro que identifica a música.
// devolve: void (chama a função de colocar a música internamente e impede q outras toquem simultaneamente)
void trocar_musica(struct jogo *j, int musica);

// entrada: jogo 
// saída: void (destrói tudo que foi alocado em jogar_jogo. função usada por menu, vitória e derrota
void destruir_gameplay(struct jogo *j);

// inicializa o menu e desaloca estruturas de outras fases (ex: player da gameplay)
// entrada: jogo; saída: void
void menu_jogo(struct jogo *j);

// cria as estruturas que estarão presentes no jogo, como o personagem
// entrada: jogo; saída: void
void jogar_jogo(struct jogo *j);

// tela de vitória
// entrada: jogo; saída: void
void vitoria_jogo(struct jogo *j);

// tela de derrota
// entrada: jogo; saída: void
void derrota_jogo(struct jogo *j);

// escolhe entre as opções acima (menu, jogo, tela de vitória ou de derrota) e chama as funções de atualização, desenho etc.
// entrada: jogo; saída: void
void rodar_jogo(struct jogo *j);

// desaloca todas as estruturas, imagens e áudios usados no jogo
// entrada: jogo; saída: void
void destruir_jogo(struct jogo *j);

#endif
