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

static void Dispenser_free(Dispenser*);

Dispenser* Dispenser_create(Dispenser_Id id, char* product, Battery battery, Filling filling) {
	Dispenser * this;
	this = (Dispenser *) malloc (sizeof(Dispenser));
	this->battery = 0;
	this->filling = 0;
	this->id = 0;
	this->last_wash_date = NULL;
	this->product = NULL;
	this->next_dispenser = NULL;
	this->message = NULL;
	Dispenser_set_id(this, id);
	Dispenser_set_filling(this, filling);
	Dispenser_set_battery(this, battery);
	Dispenser_set_product(this, product);
	Dispenser_set_current_date(this);
	return this;
}

void Dispenser_destroy(Dispenser* this) {
	if(this != NULL) {
		Dispenser_free(this);
	}
}

static void Dispenser_free(Dispenser* this) {
	this->id = 0;
	this->battery = 0;
	this->filling = 0;
	this->next_dispenser = NULL;
	Product_destroy(this->product);
	this->product = NULL;
	Date_destroy(this->last_wash_date);
	this->last_wash_date = NULL;
	MessageToSend_destroy(this->message);
	this->message = NULL;
	free(this);
}

/* Getter */

char* Dispenser_get_product_name(Dispenser* this) {
	assert(this != NULL);
	char* ptr_to_name = Product_get_name(this->product);
	return ptr_to_name;
}

uint8_t Dispenser_get_product_size(Dispenser* this) {
	assert(this != NULL);
	uint8_t size = Product_get_size(this->product);
	return size;
}

Product* Dispenser_get_product(Dispenser* this) {
	assert(this != NULL);
	return this->product;
}

Battery Dispenser_get_battery(Dispenser* this) {
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

	bool existing_message = MessageToSend_exist(this->message);
	if(!existing_message) {
		return NULL;
	}
	return this->message;
}

Date* Dispenser_get_last_wash_date(Dispenser* this) {
	assert(this != NULL);

	return this->last_wash_date;
}

int Dispenser_get_year(Dispenser* this) {
	assert(this != NULL);
	int ret = Date_get_year(this->last_wash_date);
	assert(ret > 1900);
	return ret;
}

int Dispenser_get_day(Dispenser* this) {
	assert(this != NULL);
	int ret = Date_get_day(this->last_wash_date);
	assert(ret >= 0 && ret <= 366);
	return ret;
}

/* Setter */

void Dispenser_set_product(Dispenser* this, char* product) {
	assert(this != NULL);
	if(this->product != NULL) {
		Product_destroy(this->product);
	}
	this->product = Product_set_product(product);
}

void Dispenser_set_battery(Dispenser* this, Battery battery) {
	assert(this != NULL);

	this->battery = battery;
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
		Date_destroy(this->last_wash_date);
	}
	this->last_wash_date = Date_set_current_date();
}

bool Dispenser_is_last_dispenser(Dispenser* this) {
	assert(this != NULL);
	return (this->next_dispenser == NULL);
}

void Dispenser_printf(Dispenser* this, char* arg) {
	assert(this != NULL);
	printf("\n---------------------------------------------------------------------------------\n");

	char* product = Dispenser_get_product_name(this);
	printf("Dispenser numéro %d : contient du %s, est rempli à %d %% et sa batterie est à %d %%\n", this->id, product, this->filling, this->battery);
	if(strcmp(arg, "v") == 0) {
		printf("Date dernier lavage : %d de l\' année %d\n", this->last_wash_date->current_day, this->last_wash_date->year);
		printf("message en queue pour le dispenser : %s\n", this->message);
	}
	printf("---------------------------------------------------------------------------------\n");

}
