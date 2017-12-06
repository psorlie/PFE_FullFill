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
#include "DispenserManager.h"
#include "Dispenser.h"
#include "Application.h"

int main(void) {
	printf("!!!Hello World!!!\n"); /* prints !!!Hello World!!! */
	DispenserManager_add_dispenser(1, "cacao", 2, 3);
	DispenserManager_printf("v");
	DispenserManager_add_dispenser(2, "chocolat", 4, 6);
	DispenserManager_printf("v");
	DispenserManager_add_dispenser(30, "gnoutu", 40, 60);
	DispenserManager_printf("v");
	DispenserManager_add_dispenser(5, "coucou wesh", 18, 18);
	DispenserManager_printf("v");
	printf(" ~~~~~~~~~~~~~~~~~~~~~~~~ \n");
	printf(" ~~~~~~~~~~~~~~~~~~~~~~~~ \n");
	printf(" ~~~~~~~~~~~~~~~~~~~~~~~~ \n");
	printf(" ~~~~~~~~~~~~~~~~~~~~~~~~ \n");
	printf(" ~~~~~~~~~~~~~~~~~~~~~~~~ \n");
	printf(" ~~~~~~~~~~~~~~~~~~~~~~~~ \n");
	DispenserManager_free_dispenser(1);
	DispenserManager_printf("v");
	DispenserManager_free_dispenser(30);
	DispenserManager_printf("v");
	DispenserManager_free_dispenser(5);
	DispenserManager_printf("v");
	DispenserManager_free_dispenser(2);
	DispenserManager_printf("v");
	DispenserManager_add_dispenser(1, "cacao", 2, 3);
	DispenserManager_printf("v");

	// Implemented : Date, Id, Filling, Battery, Product / Managing linked list too
	return EXIT_SUCCESS;
}
