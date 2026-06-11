CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -pedantic
ALLEGRO_LIBS = -lallegro -lallegro_main -lallegro_font -lallegro_ttf -lallegro_audio -lallegro_acodec -lallegro_image
PROGRAM = omori

all: $(PROGRAM)

$(PROGRAM): main.o Bonus.o Plataformas.o Cenario.o Jogo.o Joystick.o Obstaculos.o Omori.o
	$(CC) -o $(PROGRAM) $^ $(CFLAGS) $(ALLEGRO_LIBS)

main.o: main.c
	$(CC) -c $^ $(CFLAGS) $(ALLEGRO_LIBS)

Bonus.o: Bonus.c
	$(CC) -c $^ $(CFLAGS) $(ALLEGRO_LIBS) 

Plataformas.o: Plataformas.c
	$(CC) -c $^ $(CFLAGS) $(ALLEGRO_LIBS)

Cenario.o: Cenario.c
	$(CC) -c $^ $(CFLAGS) $(ALLEGRO_LIBS)

Jogo.o: Jogo.c
	$(CC) -c $^ $(CFLAGS) $(ALLEGRO_LIBS)

Joystick.o: Joystick.c
	$(CC) -c $^ $(CFLAGS) $(ALLEGRO_LIBS)

Obstaculos.o: Obstaculos.c
	$(CC) -c $^ $(CFLAGS) $(ALLEGRO_LIBS)

Omori.o: Omori.c
	$(CC) -c $^ $(CFLAGS) $(ALLEGRO_LIBS)

clean:
	rm -f *.gch *.o $(PROGRAM)
