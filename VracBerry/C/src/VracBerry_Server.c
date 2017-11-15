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
	Dispenser* future_dispenser;
	future_dispenser = Dispenser_create(1, "cacao", 2, 3);
	int ret =  Dispenser_get_id(future_dispenser);
	int taille_nom = Dispenser_get_product_size(future_dispenser);
	char* nom_produit = Dispenser_get_product_name(future_dispenser);
	printf("\n id : %d, taille_nom : %d, nom_produit : %s", ret, taille_nom, nom_produit);
	Dispenser_set_product(future_dispenser,"chocolat");
	int taille_nom_2 = Dispenser_get_product_size(future_dispenser);
	char* nom_produit_2 = Dispenser_get_product_name(future_dispenser);
	printf("\n id : %d, taille_nom : %d, nom_produit : %s", ret, taille_nom_2, nom_produit_2);
	// Implemented : Date, Id, Filling, Battery, Product

	return EXIT_SUCCESS;
}
