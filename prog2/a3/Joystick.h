#ifndef __JOYSTICK__ 
#define __JOYSTICK__

typedef struct {			//Definição da estrutura de um controle 
	unsigned char right;		//Botão de movimentação à direta 
	unsigned char left;		//Botão de movimentação à esquerda 
	unsigned char jump;		//Botão de movimentação para cima 
	unsigned char down;		//Botão de movimentação para baixo
} joystick;				//Definição do nome da estrutura 

/* aloca memória na heap para um novo controle */
joystick* joystick_create();	

// funções de press fazem com que a movimentação ocorra enquanto está setado no 1
// funções de release liberam a função depois que a tecla parou de ser pressionada
void joystick_press_left(joystick *element); 
void joystick_release_left(joystick *element);
void joystick_press_right(joystick *element);
void joystick_release_right(joystick *element);
void joystick_press_jump(joystick *element);
void joystick_release_jump(joystick *element);
void joystick_press_down(joystick *element);
void joystick_release_down(joystick *element);

void joystick_destroy(joystick *element); //libera a memória do elemento da heap

#endif
