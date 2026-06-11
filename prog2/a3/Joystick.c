#include <stdlib.h>
#include "Joystick.h"

joystick* joystick_create(){														

	joystick *element = (joystick*) malloc (sizeof(joystick));						
	if (!element) return NULL;
	/* insere o estado de desligado para o botão de movimentação para cada um dos lados (direita, esquerda, acima, abaixo) */
	element->right = 0;																
	element->left = 0;										
	element->jump = 0;								
	element->down = 0;													
	return element;	/* retorna o novo controle */
}				

void joystick_press_left(joystick *element)    { element->left  = 1; }

void joystick_release_left(joystick *element)  { element->left  = 0; }

void joystick_press_right(joystick *element)   { element->right = 1; }

void joystick_release_right(joystick *element) { element->right = 0; }

void joystick_press_jump(joystick *element)    { element->jump  = 1; }

void joystick_release_jump(joystick *element)  { element->jump  = 0; }

void joystick_press_down(joystick *element)    { element->down  = 1; }

void joystick_release_down(joystick *element)  { element->down  = 0; }

void joystick_destroy(joystick *element) { 
	free(element);
}			
