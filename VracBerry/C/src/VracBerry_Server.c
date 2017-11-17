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

int main(void) {
	printf("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	DispenserManager_add_dispenser(1, "cacao", 2, 3);
	DispenserManager_add_dispenser(10, "chocolat", 20, 30);
	DispenserManager_add_dispenser(30, "gnoutu", 40, 60);
	printf("\n avant free");
	DispenserManager_free_dispenser(10);
	printf("\n après free");
	DispenserManager_add_dispenser(1, "cacao", 2, 3);
	DispenserManager_add_dispenser(10, "chocolat", 20, 30);
	DispenserManager_add_dispenser(30, "gnoutu", 40, 60);

	int resultat = DispenserManager_get_filling(10);
	printf("\n ------on teste la récup du premier : %d--------", resultat);

	Dispenser* testastos = DispenserManager_get_list();
	Dispenser* apres = testastos->next_dispenser;
	Dispenser*encore_apres = apres->next_dispenser;
	printf("\n un : %d, deux : %d, trois : %d", (int)testastos, (int)apres, (int)encore_apres);


	DispenserManager_add_dispenser(10, "chocolat", 20, 30);

	printf("\n après nouveau add");

	int ptr_debut_liste = (int)DispenserManager_get_list();
	int middle_filling = DispenserManager_get_filling(10);
	int ptr_middle = (int)DispenserManager_find_dispenser(10);
	printf("\n middle_added filling : %d et ptr : et ptr debut : %d", middle_filling, ptr_middle, ptr_debut_liste);


	DispenserManager_add_dispenser(69, "pizza", 68, 67);

	DispenserManager_free_dispenser(1);

	int last_added_filling = DispenserManager_get_filling(69);
	printf("\n last_added filling : %d", last_added_filling);

	int testas = DispenserManager_get_filling(10);
	printf("\n testas : %d \n", testas);

	DispenserManager_free_dispenser(69);

	testas = DispenserManager_get_filling(10);
	printf("\n testas : %d", testas);
	// Implemented : Date, Id, Filling, Battery, Product / Managing linked list too

	return EXIT_SUCCESS;
}
