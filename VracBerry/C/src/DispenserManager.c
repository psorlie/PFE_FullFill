/*
 * DispenserManager.c
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#include "Dispenser.h"

struct Dispenser_list_t {
	Dispenser* first_dispenser;
};

static Dispenser_list* dispenser_list;

Dispenser_list* DispenserManager_initialisation(){
	assert(dispenser_list == NULL);

	Dispenser_list* list = malloc(sizeof(Dispenser_list));
	if(list == NULL){
		exit(0);
	}
	return list;
}

DispenserManager_add_dispenser(Dispenser_Id id, Product product, Battery battery, Filling filling) {
	if(dispenser_list == NULL) {
		dispenser_list = DispenserManager_initialisation();
	}
	Dispenser* new_dispenser = Dispenser_create(id, product, battery, filling);
	new_dispenser->next_dispenser = dispenser_list->first_dispenser;
	dispenser_list->first_dispenser = new_dispenser;
}

void DispenserManager_free_dispenser(Dispenser_Id id) {
	assert(dispenser_list != NULL);
	Dispenser* before_deleted_dispenser = DispenserManager_find_before_dispenser(id);
	Dispenser* future_deleted_dispenser = before_deleted_dispenser->next_dispenser;

	before_deleted_dispenser->next_dispenser = future_deleted_dispenser->next_dispenser;
	Dispenser_free(future_deleted_dispenser);
}

Dispenser* DispenserManager_find_before_dispenser(Dispenser_Id id) {
	assert(dispenser_list != NULL);
	int has_been_found = 0;

	Dispenser* current_dispenser = dispenser_list->first_dispenser;
	if(current_dispenser->id != id){
		has_been_found = 1;
	}
		while (current_dispenser->next_dispenser != NULL && !has_been_found) {

			if(current_dispenser->next_dispenser->id == id){
				has_been_found = 1;
			} else {
				current_dispenser = current_dispenser->next_dispenser;
			}
		}

	return current_dispenser;

}
