/*
 * DispenserManager.c
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#include <assert.h>
#include <stdlib.h>

#include "Dispenser.h"
#include "DispenserManager.h"

struct Dispenser_list_t {
	Dispenser* first_dispenser;
};

static Dispenser_list* dispenser_list;

static Dispenser* DispenserManager_find_before_dispenser(Dispenser_Id);

static Dispenser_list* DispenserManager_initialisation();

static bool DispenserManager_exist(Dispenser_Id);

static Dispenser_list* DispenserManager_initialisation() {
	assert(dispenser_list == NULL);

	Dispenser_list* list = malloc(sizeof(Dispenser_list));
	if(list == NULL){
		exit(0);
	}
	return list;
}

static bool DispenserManager_exist(Dispenser_Id id) {
	assert(dispenser_list != NULL);
	bool has_been_found = false;

	Dispenser* current_dispenser = dispenser_list->first_dispenser;
	if(current_dispenser->id != id){
		has_been_found = true;
	}
		while (current_dispenser->next_dispenser != NULL && !has_been_found) {

			if(current_dispenser->next_dispenser->id == id){
				has_been_found = true;
			} else {
				current_dispenser = current_dispenser->next_dispenser;
			}
		}
	return has_been_found;

}

void DispenserManager_add_dispenser(Dispenser_Id id, char* product, Battery battery, Filling filling) {
	if(dispenser_list == NULL) {
		dispenser_list = DispenserManager_initialisation();
	}
	Dispenser* new_dispenser = Dispenser_create(id, product, battery, filling);
	new_dispenser->next_dispenser = dispenser_list->first_dispenser;
	dispenser_list->first_dispenser = new_dispenser;
}

Dispenser* DispenserManager_get_list(){
	return dispenser_list->first_dispenser;
}

void DispenserManager_free_dispenser(Dispenser_Id id) {
	assert(dispenser_list != NULL);
	Dispenser* before_deleted_dispenser;
	Dispenser* future_deleted_dispenser;

	if(dispenser_list->first_dispenser->id == id) {
		future_deleted_dispenser = dispenser_list->first_dispenser;
		dispenser_list->first_dispenser = dispenser_list->first_dispenser->next_dispenser;
		printf("premier : %d, deuxieme : %d", (int)dispenser_list->first_dispenser, (int)dispenser_list->first_dispenser->next_dispenser);

	} else {
		before_deleted_dispenser = DispenserManager_find_before_dispenser(id);
		future_deleted_dispenser = before_deleted_dispenser->next_dispenser;
		before_deleted_dispenser->next_dispenser = future_deleted_dispenser->next_dispenser;
	}
	//Dispenser_free(future_deleted_dispenser);
	printf("\n \n premier : %d, deuxieme : %d", (int)dispenser_list->first_dispenser, (int)dispenser_list->first_dispenser->next_dispenser);

}

static Dispenser* DispenserManager_find_before_dispenser(Dispenser_Id id) {
	assert(dispenser_list != NULL);
	bool has_been_found = false;

	Dispenser* current_dispenser = dispenser_list->first_dispenser;
	if(current_dispenser->id != id){
		has_been_found = true;
	}
	while (current_dispenser->next_dispenser != NULL && !has_been_found) {
		if(current_dispenser->next_dispenser->id == id){
			has_been_found = true;
		} else {
			current_dispenser = current_dispenser->next_dispenser;
		}
	}
	if (!has_been_found) {
		current_dispenser = (void*)NULL;
	}
	return current_dispenser;
}

Dispenser* DispenserManager_find_dispenser(Dispenser_Id id) {
	assert(dispenser_list != NULL);
	bool has_been_found = false;

	Dispenser* current_dispenser = dispenser_list->first_dispenser;
		while (current_dispenser->next_dispenser != NULL && !has_been_found) {
			if(current_dispenser->id == id){
				has_been_found = true;
			} else {
				current_dispenser = current_dispenser->next_dispenser;
			}
		}
		if (!has_been_found){
			current_dispenser = (void*) NULL;
		}
		return current_dispenser;
}

void DispenserManager_update_battery(Dispenser_Id id, Battery battery) {
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	Dispenser_set_battery(dispenser, battery);
}

void DispenserManager_update_filling(Dispenser_Id id, Filling filling) {
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	Dispenser_set_filling(dispenser, filling);
}

void DispenserManager_update_product(Dispenser_Id id, char* product_name) {
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	Dispenser_set_product(dispenser, product_name);
}

void DispenserManager_update_last_wash_date(Dispenser_Id id) {
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	Dispenser_set_current_date(dispenser);
}

Battery DispenserManager_get_battery(Dispenser_Id id) {
	Battery battery = 0;
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	if(dispenser != NULL){
		battery = Dispenser_get_battery(dispenser);
	}
	return battery;
}

Filling DispenserManager_get_filling(Dispenser_Id id) {
	Filling filling = 0;
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	if(dispenser != NULL) {
		filling = Dispenser_get_filling(dispenser);
	}
	return filling;
}

char* DispenserManager_get_product(Dispenser_Id id) {
	char* product_name;
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	if(dispenser != NULL) {
		product_name = Dispenser_get_product_name(dispenser);
	} else {
		product_name = "don\'t exist";
	}
	return product_name;
}

int DispenserManager_get_cleaning_day(Dispenser_Id id ) {
	int day = -1;
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	if(dispenser != NULL) {
		day = Dispenser_get_day(dispenser);
	}
	return day;
}

int DispenserManager_get_cleaning_year(Dispenser_Id id ) {
	int year = -1;
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	if(dispenser != NULL) {
		year = Dispenser_get_year(dispenser);
	}
	return year;
}
