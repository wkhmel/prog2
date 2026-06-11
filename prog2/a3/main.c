// gcc main.c Jogo.c Omori.c Joystick.c Cenario.c Obstaculos.c -o jogo $(pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5 allegro_audio-5 allegro_acodec-5 --libs --cflags)#include "Jogo.h"

//gcc main.c Jogo.c Omori.c Joystick.c Cenario.c Obstaculos.c -o jogo $(pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_ttf-5 allegro_primitives-5 allegro_image-5 allegro_audio-5 allegro_acodec-5 --libs --cflags)
#include "Jogo.h"

int main() {
    struct jogo *j = criar_jogo();
    if (!j) return 1;

    rodar_jogo(j);
    destruir_jogo(j);
    return 0;
}
