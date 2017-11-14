/*
 * Dispenser.c
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "Dispenser.h"

Dispenser* Dispenser_create(Dispenser_Id id, char* product, Battery battery, Filling filling) {
	Dispenser * this;
	printf("bordel");
	this = (Dispenser *) malloc (sizeof(Dispenser));
	printf("oui");

	Dispenser_set_id(this, id);
	Dispenser_set_filling(this, filling);
	Dispenser_set_battery(this, battery);
	//Dispenser_set_product(this, product);
	//Dispenser_set_current_date(this);
	this->next_dispenser = NULL;
	printf("Ph il est bo");
	return this;
}

void Dispenser_free(Dispenser* this){
	if(this->last_wash_date != NULL){
		free(this->last_wash_date);
	}
	free(this);
}
/* Getter */

Product* Dispenser_get_product(Dispenser* this) {
	assert(this != NULL);
	Product* product = this->product;
	return product;
}

Battery Dispenser_get_battery(Dispenser* this) {
	printf("\n coucou moi je %d lolilol", (int)this);
	assert(this != NULL);
	return this->battery;
}

Dispenser_Id Dispenser_get_id(Dispenser* this) {
	assert(this != NULL);

	return this->id;
}

Filling Dispenser_get_filling(Dispenser* this) {
	assert(this != NULL);

	return this->filling;
}

MessageToSend* Dispenser_get_message(Dispenser* this) {
	assert(this != NULL);

	return this->message;
}

Date* Dispenser_get_last_wash_date(Dispenser* this) {
	assert(this != NULL);

	return this->last_wash_date;
}

/* Setter */

void Dispenser_set_product(Dispenser* this, char* product) {
	assert(this != NULL);
	Product_setProduct(this->product, product);
}

void Dispenser_set_battery(Dispenser* this, Battery battery) {
	printf("hello");
	assert(this != NULL);

	this->battery = battery;
	printf("fin de set batterie");
}

void Dispenser_set_id(Dispenser* this, Dispenser_Id id) {
	assert(this != NULL);

	this-> id = id;
}

void Dispenser_set_filling(Dispenser* this, Filling filling) {
	assert(this != NULL);

	this->filling = filling;
}

void Dispenser_set_message(Dispenser* this, MessageToSend* message) {
	assert(this != NULL);

	this->message = message;
}

void Dispenser_set_current_date(Dispenser* this) {
	assert(this != NULL);

	if(this->last_wash_date != NULL) {
		Date_free(this->last_wash_date);
	}
	Date_set_current_date(this->last_wash_date);
}

bool Dispenser_is_last_dispenser(Dispenser* this) {
	return (this->next_dispenser == NULL);
}
