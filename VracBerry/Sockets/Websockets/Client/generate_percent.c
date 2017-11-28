/*** en remplacement de l'envoi du pourcentage de remplissage ****/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "generate_percent.h"

// On suppose a<b
int rand_a_b(int a, int b){
    return rand()%(b-a) +a;
}

/* string_to_send : pointeur vers la chaine à envoyer au serveur; size : taille de la chaine en question */
size_t get_value(char* string_to_send){
	size_t size;
	int percentage = rand_a_b(0, 100);
	size = sprintf( (char*)string_to_send, "%d", percentage);
	return size;
}