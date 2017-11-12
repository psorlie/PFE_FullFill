/*
 ============================================================================
 Name        : VracBerry_Server.c
 Author      : Fullfill_team
 Version     :
 Copyright   : BSD-2clauses
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "Dispenser.h"

int main(void) {
	printf("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	Dispenser * future_dispenser;
	future_dispenser = Dispenser_create(future_dispenser, 1, "cacao", 2, 3);
	return EXIT_SUCCESS;
}
