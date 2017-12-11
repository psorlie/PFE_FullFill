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
#include "Application.h"
#include "Postman.h"

int main(void) {
	printf("!!!Hello World!!!\n"); /* prints !!!Hello World!!! */
	//Postman_init();
	sleep(1);
	printf("je vais essayer !\n");
	//Postman_send("bretzel au nutella\0");

	DispenserManager_init();
	DispenserManager_add_dispenser(1, "cacao", 2, 3);
	/*DispenserManager_add_dispenser(2, "chocolat", 4, 6);
	Dispenser* this = DispenserManager_find_dispenser(1);
	Application_update_dispenser(this);
	this = DispenserManager_find_dispenser(2);
	Application_detailed_dispenser(this);
	/*DispenserManager_printf("v");
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
	DispenserManager_printf("v"); */

	// Implemented : Date, Id, Filling, Battery, Product / Managing linked list too
	return EXIT_SUCCESS;
}
