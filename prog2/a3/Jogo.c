#include "Jogo.h"

struct jogo *criar_jogo() {
	struct jogo *j = (struct jogo*)malloc(sizeof(struct jogo));

	//varias verificações de erro em chamadas de função do allegro
	if (!j) {
		fprintf(stderr, "Erro: nao foi possivel alocar memoria para o jogo.\n");
		exit(1);
	}

	if (!al_init()) {
		fprintf(stderr, "Erro: nao foi possivel inicializar a biblioteca Allegro.\n");
		free(j);
		return NULL;
	}

	if (!al_init_image_addon()) {
		fprintf(stderr, "Erro: nao foi possivel inicializar o addon de imagens da biblioteca.\n");
		free(j);
		return NULL;
	}

	al_init_font_addon();
	al_init_ttf_addon();

	if (!al_install_keyboard()) {
		fprintf(stderr, "Erro: nao foi possivel instalar o teclado da biblioteca.\n");
		free(j);
		return NULL;
	}

	//inicialização do áudio
	al_install_audio();
	ALLEGRO_MIXER *mixer = al_get_default_mixer();
	if (mixer) al_set_mixer_frequency(mixer, 44100);
	
	al_init_acodec_addon();
	al_reserve_samples(16);
	
	j->timer = al_create_timer(1.0 / 30.0); //30fps
	j->queue = al_create_event_queue();
	j->display = al_create_display(X_SCREEN, Y_SCREEN);

	//carregando as músicas de cada etapa
	j->musicas[MENU] = al_load_audio_stream("assets/audio/ByYourSide.ogg", 8, 16384); 
	j->musicas[JOGAR] = al_load_audio_stream("assets/audio/ThreeBarLogos.ogg", 8, 16384);
	j->musicas[VITORIA] = al_load_audio_stream("assets/audio/100sunny.ogg", 8, 16384);
	j->musicas[DERROTA] = al_load_audio_stream("assets/audio/Acrophobia.ogg", 8, 16384);
	
	j->sons[MENU_SCROLL] = al_load_sample("assets/audio/scroll.ogg");
	j->sons[MENU_SELECIONAR] = al_load_sample("assets/audio/select.ogg");
	j->sons[SOM_DANO] = al_load_sample("assets/audio/dano.ogg");
	j->sons[SOM_PULO] = al_load_sample("assets/audio/pulo.ogg");
	j->sons[SOM_BONUS] = al_load_sample("assets/audio/bonus.ogg");

	if (!j->timer || !j->queue || !j->display) {
		fprintf(stderr, "Erro: algum recurso básico do Allegro nao foi criado.\n");
		destruir_jogo(j);
		return NULL;
	}

	for (int i = 0; i < QTD_ESTADOS; i++) {
		if (!j->musicas[i]) {
			fprintf(stderr, "Erro: nao foi possivel carregar a musica %d.\n", i);
			destruir_jogo(j);
			return NULL;
		}
		else {
			al_attach_audio_stream_to_mixer(j->musicas[i], al_get_default_mixer());
			al_set_audio_stream_playing(j->musicas[i], false);
		}
	}

	for (int i = 0; i < QTD_SONS; i++) {
		if (!j->sons[i]) {
			fprintf(stderr, "Erro: nao foi possivel carregar o som %d.\n", i);
			destruir_jogo(j);
			return NULL;
		}	
	}
	
	j->font = al_load_font("./assets/fonts/Secundaria.ttf", 40, 0);
	if (!j->font) {
		j->font = al_create_builtin_font();
	}

	j->titulo = al_load_font("./assets/fonts/Fonteomori.ttf", 70, 0);
	if (!j->titulo) {
		j->titulo = al_create_builtin_font();
		j->font = al_create_builtin_font(); // pra ficar padronizado
	}
	
	al_register_event_source(j->queue, al_get_keyboard_event_source());
	al_register_event_source(j->queue, al_get_display_event_source(j->display));
	al_register_event_source(j->queue, al_get_timer_event_source(j->timer));

	j->img_cenarios[CENARIO_MENU] = al_load_bitmap("./assets/images/menu.png");
	j->img_cenarios[CENARIO_JOGANDO] = al_load_bitmap("./assets/images/jogo.png");
	j->img_cenarios[CENARIO_VITORIA] = al_load_bitmap("./assets/images/youwon.png");
	j->img_cenarios[CENARIO_DERROTA] = al_load_bitmap("./assets/images/youlost.png");

	j->cenarios[CENARIO_MENU] = criar_cenario(j->img_cenarios[CENARIO_MENU], X_SCREEN);
	j->cenarios[CENARIO_JOGANDO] = criar_cenario(j->img_cenarios[CENARIO_JOGANDO], LARGURA_MAPA);
	j->cenarios[CENARIO_VITORIA] = criar_cenario(j->img_cenarios[CENARIO_VITORIA], X_SCREEN);
	j->cenarios[CENARIO_DERROTA] = criar_cenario(j->img_cenarios[CENARIO_DERROTA], X_SCREEN);
	
	j->estado_atual = MENU;
	j->OpcaoSelecionada = INICIAR;

	j->inicio_obstaculo = NULL;
	j->inicio_plataforma = NULL;
	j->inicio_bonus = NULL;
	j->omori = NULL;
	j->fullheart = j->brokenheart = NULL;
	j->chao = NULL;

	j->fullheart = al_load_bitmap("./assets/images/fullheart.png");
	j->brokenheart = al_load_bitmap("./assets/images/brokenheart.png");
		
	for (int i = 0; i < VIDA_MAXIMA; i++) {
		j->img_hearts[i] = j->fullheart;
	}
                		
	for (int i = VIDA_MAXIMA; i < 2*VIDA_MAXIMA; i++) {
		j->img_hearts[i] = j->brokenheart;
	}

	//imagem chão
	j->img_chao = al_load_bitmap("./assets/images/chao.png");
	
	//imagens omori
	j->img_sprites[PARADO] = al_load_bitmap("./assets/images/player-1.png");
	j->img_sprites[ANDANDO1] = al_load_bitmap("./assets/images/player-2.png");
	j->img_sprites[ANDANDO2] = al_load_bitmap("./assets/images/player-3.png");
	j->img_sprites[ANDANDO3] = al_load_bitmap("./assets/images/player-4.png");
	j->img_sprites[AGACHADO] = al_load_bitmap("./assets/images/player-5.png");
	j->img_sprites[PULANDO] = al_load_bitmap("./assets/images/player-6.png");

	//imagens obstáculos
	j->img_obstaculos[0] = al_load_bitmap("./assets/images/basil.png");
	j->img_obstaculos[1] = al_load_bitmap("./assets/images/banana.png");
	j->img_obstaculos[2] = al_load_bitmap("./assets/images/something.png");
	j->img_obstaculos[3] = al_load_bitmap("./assets/images/espinhos.png");
	j->img_obstaculos[4] = al_load_bitmap("./assets/images/veneno.png");
	j->img_obstaculos[5] = al_load_bitmap("./assets/images/abelhavoadora.png");

	for (int i = 0; i < QTD_OBSTACULOS; i++) {
		if (!j->img_obstaculos[i]) printf("OBSTACULO N CARREGADO");
	}

	//imagens plataformas
	j->img_plataformas[0] = al_load_bitmap("./assets/images/plataforma1.png");
	j->img_plataformas[1] = al_load_bitmap("./assets/images/plataforma2.png");
	j->img_plataformas[2] = al_load_bitmap("./assets/images/plataforma3.png");

	//imagens itens bônus
	j->img_bonus[0] = al_load_bitmap("./assets/images/honey.png");
	j->img_bonus[1] = al_load_bitmap("./assets/images/applejuice.png");

	return j;
}
 

void colocar_musica(ALLEGRO_AUDIO_STREAM *musica) {
	if (!musica) return;

	// musica toca em loop enquanto o usuário estiver usando
	al_set_audio_stream_playmode(musica, ALLEGRO_PLAYMODE_LOOP);
	al_set_audio_stream_playing(musica, true);
}

void trocar_musica(struct jogo *j, int musica) {
	if (!j) return;
	
	for (int i = 0; i < QTD_ESTADOS; i++) {
		if (j->musicas[i]) {
			al_set_audio_stream_playing(j->musicas[i], false);
                }
	}

	if (j->musicas[musica]) colocar_musica(j->musicas[musica]);
}

void destruir_gameplay(struct jogo *j) {
	if (!j) return;

	//se o jogador veio da gameplay, destroi o omori + obstaculos
	if (j->omori) {
		player_destroy(j->omori);
		j->omori = NULL;
	}

	if (j->chao) {
		destruir_chao(j->chao);
		j->chao = NULL;
	}
	
	if (j->inicio_obstaculo) {
		j->inicio_obstaculo = destruir_obstaculos(j->inicio_obstaculo);
	}
	
	if (j->inicio_plataforma) {
		j->inicio_plataforma = destruir_plataforma(j->inicio_plataforma);
	}

	if (j->inicio_bonus) {
		j->inicio_bonus = destruir_itens(j->inicio_bonus);
	}
}

void menu_jogo(struct jogo *j) {
	if (!j) return;

	j->estado_atual = MENU;
	destruir_gameplay(j);
}

void jogar_jogo(struct jogo *j) {
	if (!j) return;

	j->estado_atual = JOGAR;

	// face, hitbox_largura e hitbox_altura, largura_agachado e altura_agachado, posições x e y, posições iniciais (horizontal e vertical), posições maximas, sprites e corações
	if (!j->omori) j->omori = player_create(1, 39, 62, 52, 47, 100, Y_SCREEN - DIST_SOLO - 16, 0, 0, j->cenarios[CENARIO_JOGANDO]->largura_mapa, Y_SCREEN, j->img_sprites, j->img_hearts);

	if (!j->chao) {
		j->chao = criar_chao(DIST_SOLO, j->img_chao);
	}

	if (!j->inicio_obstaculo) {
		// basil móvel
		j->inicio_obstaculo = criar_obstaculo(j->inicio_obstaculo, 500, Y_SCREEN - DIST_SOLO - 40 - PIXELS_TRANSPARENTES_CHAO, 55, 58, j->img_obstaculos[0], 70, 4, 2);
		//banana fixa
		j->inicio_obstaculo = criar_obstaculo(j->inicio_obstaculo, 250, Y_SCREEN - DIST_SOLO - 20, 40, 32, j->img_obstaculos[1], 0, 0, 1);
		//something móvel
		j->inicio_obstaculo = criar_obstaculo(j->inicio_obstaculo, 1750, Y_SCREEN - DIST_SOLO - 64, 60, 60, j->img_obstaculos[2], 100, 8, 2);
		//espinhos fixos
		j->inicio_obstaculo =  criar_obstaculo(j->inicio_obstaculo, 1300, Y_SCREEN - DIST_SOLO - 16, 32, 18, j->img_obstaculos[3], 0, 0, 1);
		//veneno mortal
		j->inicio_obstaculo = criar_obstaculo(j->inicio_obstaculo, 868, Y_SCREEN - DIST_SOLO, 184, 110, j->img_obstaculos[4], 0, 0, 5);
		//abelha voadora que obriga o omori a se agachar
		j->inicio_obstaculo = criar_obstaculo(j->inicio_obstaculo, 2700, Y_SCREEN - 180, 40, 40, j->img_obstaculos[5], 150, 4, 3);
	}

	if (!j->inicio_plataforma) {
		// plataforma em cima do veneno
		j->inicio_plataforma = criar_plataforma(j->inicio_plataforma, 867, Y_SCREEN - 180, j->img_plataformas[0], 27, 5, 111);
		j->inicio_plataforma = criar_plataforma(j->inicio_plataforma, 2020, Y_SCREEN - 200, j->img_plataformas[1], 27, 3, 50);
		//plataforma de pianinho
		j->inicio_plataforma = criar_plataforma(j->inicio_plataforma, 2072, Y_SCREEN - 290, j->img_plataformas[2], 27, 0, 0);
	}

	if (!j->inicio_bonus) {
			//potinho de mel (fica em cima da plataforma de piano)
			j->inicio_bonus = criar_item(j->inicio_bonus, 2082, Y_SCREEN - 290, j->img_bonus[0], 0.2, 2);
			//suquinho de maçã
			j->inicio_bonus = criar_item(j->inicio_bonus, 1550, Y_SCREEN - DIST_SOLO - 16, j->img_bonus[1], 1.0, 0);
	}
}

void vitoria_jogo(struct jogo *j) {
	if (!j) return;

	// estado gráfico de vitória
	j->estado_atual = VITORIA; 
	destruir_gameplay(j);
}

void derrota_jogo(struct jogo *j) {
	if (!j) return;

	//estado gráfico de derrota
	j->estado_atual = DERROTA;
	destruir_gameplay(j);
}


void rodar_jogo(struct jogo *j) {
	if (!j) return;

	al_start_timer(j->timer); //inicia relógio do jogo

	int exit = 0; // 1 = sair, 0 = não sair

	enum ESTADO_JOGO estado_anterior = j->estado_atual;
	trocar_musica(j, j->estado_atual);

	while (!exit) {
		ALLEGRO_EVENT event;
		al_wait_for_event(j->queue, &event);

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			exit = 1;
		}
		else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (j->estado_atual == MENU) {
				if (event.keyboard.keycode == ALLEGRO_KEY_DOWN || event.keyboard.keycode == ALLEGRO_KEY_S) {
					j->OpcaoSelecionada = SAIR;
					// som de scroll das opções :)
					al_play_sample(j->sons[MENU_SCROLL], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				} else if (event.keyboard.keycode == ALLEGRO_KEY_UP || event.keyboard.keycode == ALLEGRO_KEY_W) {
					j->OpcaoSelecionada = INICIAR;
					al_play_sample(j->sons[MENU_SCROLL], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				} else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
					al_play_sample(j->sons[MENU_SELECIONAR], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					if (j->OpcaoSelecionada == SAIR) {
						exit = 1;
					} else if (j->OpcaoSelecionada == INICIAR) {
						jogar_jogo(j); // inicializa mapa+omori
					}
				}
			} 
			else if (j->estado_atual == JOGAR) {
				if (event.keyboard.keycode == ALLEGRO_KEY_LEFT || event.keyboard.keycode == ALLEGRO_KEY_A) {
					joystick_press_left(j->omori->control);
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT || event.keyboard.keycode == ALLEGRO_KEY_D) {
					joystick_press_right(j->omori->control);
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_UP || event.keyboard.keycode == ALLEGRO_KEY_W || event.keyboard.keycode == ALLEGRO_KEY_SPACE) {  
					if (j->omori->NoChao) {
						joystick_press_jump(j->omori->control);
						j->omori->NoChao = false; // para o personagem não pular automaticamente
						al_play_sample(j->sons[SOM_PULO], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					}
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_DOWN || event.keyboard.keycode == ALLEGRO_KEY_S) {  
					joystick_press_down(j->omori->control);
				}
			}
			else if (j->estado_atual == VITORIA || j->estado_atual == DERROTA) {
				// na tela final, apertar enter pra voltar pro menu e esc pra sair
				if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
				menu_jogo(j); 
				} else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
					exit = 1;
					j->OpcaoSelecionada = SAIR;
				}
			}
		}

		else if (event.type == ALLEGRO_EVENT_KEY_UP) {
			if (j->estado_atual == JOGAR) {
				if (event.keyboard.keycode == ALLEGRO_KEY_LEFT || event.keyboard.keycode == ALLEGRO_KEY_A) {  
					joystick_release_left(j->omori->control);
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT || event.keyboard.keycode == ALLEGRO_KEY_D) {
					joystick_release_right(j->omori->control);
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_UP || event.keyboard.keycode == ALLEGRO_KEY_W || event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
					joystick_release_jump(j->omori->control);
				}  
				if (event.keyboard.keycode == ALLEGRO_KEY_DOWN || event.keyboard.keycode == ALLEGRO_KEY_S) {
					joystick_release_down(j->omori->control);
				}  
			}
		}
	
		else if (event.type == ALLEGRO_EVENT_TIMER) {

			if (j->estado_atual != estado_anterior) {
				trocar_musica(j, j->estado_atual);
				estado_anterior = j->estado_atual;
			}

			if (j->estado_atual == MENU) {
				al_clear_to_color(al_map_rgb(0, 0, 0));

				desenhar_cenario(j->cenarios[CENARIO_MENU], Y_SCREEN);

				al_draw_text(j->titulo, al_map_rgb(0, 0, 0), X_SCREEN/2, Y_SCREEN/2 - 80, ALLEGRO_ALIGN_CENTRE, "GO HOME, OMORI");

				// Cores com base na seleção
				ALLEGRO_COLOR cor_ini = (j->OpcaoSelecionada == INICIAR) ? al_map_rgb(255, 215, 0) : al_map_rgb(255, 255, 255);
				ALLEGRO_COLOR cor_sai = (j->OpcaoSelecionada == SAIR) ? al_map_rgb(255, 215, 0) : al_map_rgb(255, 255, 255);

				al_draw_text(j->font, cor_ini, X_SCREEN/2, Y_SCREEN/2 - 10, ALLEGRO_ALIGN_CENTRE, (j->OpcaoSelecionada == INICIAR) ? "> INICIAR <" : "INICIAR");
				al_draw_text(j->font, cor_sai, X_SCREEN/2, Y_SCREEN/2 + 20, ALLEGRO_ALIGN_CENTRE, (j->OpcaoSelecionada == SAIR) ? "> SAIR <" : "SAIR");

				al_flip_display();
			}

			else if (j->estado_atual == JOGAR) {
								
				// atualizações
				atualizar_obstaculo(j->inicio_obstaculo);
				atualizar_plataforma(j->inicio_plataforma);
				
				player_atualizaPose(j->omori);
				player_atualizaPosicao(j->omori, LARGURA_MAPA, Y_SCREEN);
				checar_plataforma(j->omori, j->inicio_plataforma);
				atualizar_cenario(j->cenarios[CENARIO_JOGANDO], j->omori, X_SCREEN);

				//essa funcao retorna 0 se o cara m tomou dano e 1 se tomou, e aq decide se vai tocar o som de dano ou n 
				if (checar_dano_obstaculos(j->inicio_obstaculo, j->omori) && j->omori->hp > 0) al_play_sample(j->sons[SOM_DANO], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				// valor retornado pela coleta também serve pra decidir se toca som de bonus ou n
				if (checar_coleta_itens(j->inicio_bonus, j->omori)) al_play_sample(j->sons[SOM_BONUS], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

				if (j->omori && j->omori->hp == 0) {
					derrota_jogo(j);
				}
				else if (j->omori && j->omori->x >= LARGURA_MAPA - 150) {
					vitoria_jogo(j);
				}

				if (j->estado_atual == JOGAR) {
					al_clear_to_color(al_map_rgb(0, 0, 0));

					desenhar_cenario(j->cenarios[CENARIO_JOGANDO], Y_SCREEN); 
					desenhar_chao(j->chao, j->cenarios[CENARIO_JOGANDO]->camera_x);
					player_draw(j->omori, j->cenarios[CENARIO_JOGANDO]->camera_x);
					desenhar_plataforma(j->inicio_plataforma, j->cenarios[CENARIO_JOGANDO]->camera_x);
					desenhar_obstaculo(j->inicio_obstaculo, j->cenarios[CENARIO_JOGANDO]->camera_x);
					desenhar_itens(j->inicio_bonus, j->cenarios[CENARIO_JOGANDO]->camera_x);
					
					al_flip_display();
				}
			}

			else if (j->estado_atual == VITORIA) {
				al_clear_to_color(al_map_rgb(0, 0, 0));
                                
				desenhar_cenario(j->cenarios[CENARIO_VITORIA], Y_SCREEN);

				al_draw_text(j->titulo, al_map_rgb(255, 255, 255), X_SCREEN/2, Y_SCREEN/2 - 30, ALLEGRO_ALIGN_CENTRE, "WE KNEW YOU WOULD COME BACK, SUNNY!");
				al_draw_text(j->font, al_map_rgb(255, 255, 255), X_SCREEN/2, Y_SCREEN/2 + 10, ALLEGRO_ALIGN_CENTRE, "Pressione ENTER para voltar ao Menu");
				al_draw_text(j->font, al_map_rgb(255, 255, 255), X_SCREEN/2, Y_SCREEN/2 + 35, ALLEGRO_ALIGN_CENTRE, "Pressione ESC para sair");
				
				al_flip_display();
			}

			else if (j->estado_atual == DERROTA) {
				al_clear_to_color(al_map_rgb(0, 0, 0));
				
				desenhar_cenario(j->cenarios[CENARIO_DERROTA], Y_SCREEN);	                        
        
				al_draw_text(j->titulo, al_map_rgb(255, 255, 255), X_SCREEN/2, Y_SCREEN/2 - 30, ALLEGRO_ALIGN_CENTRE, "GUESS IT WAS JUST A DREAM...");
				al_draw_text(j->font, al_map_rgb(255, 255, 255), X_SCREEN/2, Y_SCREEN/2 + 10, ALLEGRO_ALIGN_CENTRE, "Pressione ENTER para voltar ao Menu");
				al_draw_text(j->font, al_map_rgb(255, 255, 255), X_SCREEN/2, Y_SCREEN/2 + 20, ALLEGRO_ALIGN_CENTRE, "Pressione ESC para sair");

				al_flip_display();
			}
		}
	}
}

void destruir_jogo(struct jogo *j) {
	if (!j) return;
	
	if (j->omori) player_destroy(j->omori);
        
	if (j->inicio_obstaculo) {
		j->inicio_obstaculo = destruir_obstaculos(j->inicio_obstaculo);
	}

	if (j->inicio_plataforma) {
		j->inicio_plataforma = destruir_plataforma(j->inicio_plataforma);	
	}
	
	for (int i = 0; i < QTD_OBSTACULOS; i++) {
		if (j->img_obstaculos[i]) al_destroy_bitmap(j->img_obstaculos[i]);
	}
	
	for (int i = 0; i < QTD_PLATAFORMAS; i++) {
		if (j->img_plataformas[i]) al_destroy_bitmap(j->img_plataformas[i]);
	}

	for (int i = 0; i < QTD_BONUS; i++) {
		if (j->img_bonus[i]) al_destroy_bitmap(j->img_bonus[i]);
	}

	for (int i = 0; i < NUM_POSES; i++)
		if (j->img_sprites[i]) al_destroy_bitmap(j->img_sprites[i]);

	for (int i = 0; i < QTD_CENARIOS; i++) {
		if (j->cenarios[i]) destruir_cenario(j->cenarios[i]);
	}

	if (j->chao) destruir_chao(j->chao);
	if (j->img_chao) al_destroy_bitmap(j->img_chao);
	if (j->fullheart) al_destroy_bitmap(j->fullheart);
	if (j->brokenheart) al_destroy_bitmap(j->brokenheart);

	for (int i = 0; i < QTD_CENARIOS; i++)
		if (j->img_cenarios[i]) al_destroy_bitmap(j->img_cenarios[i]);

	for (int i = 0; i < QTD_ESTADOS; i++) {
		if (j->musicas[i]) al_destroy_audio_stream(j->musicas[i]);
	}

	for (int i = 0; i < QTD_SONS; i++) {
		if (j->sons[i]) al_destroy_sample(j->sons[i]);
	}
	
	if (j->font) al_destroy_font(j->font);
	if (j->titulo) al_destroy_font(j->titulo);
	al_destroy_display(j->display);
	al_destroy_timer(j->timer);
	al_destroy_event_queue(j->queue);

	free(j);
}

