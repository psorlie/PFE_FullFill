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
	this = (Dispenser *) malloc (sizeof(Dispenser));

	Dispenser_set_id(this, id);
	Dispenser_set_filling(this, filling);
	Dispenser_set_battery(this, battery);
	Dispenser_set_product(this, product);
	Dispenser_set_current_date(this);
	this->next_dispenser = NULL;
	return this;
}

void Dispenser_free(Dispenser* this){
/*	Date_free(this->last_wash_date);
	this->last_wash_date= NULL;
	Product_destroy(this->product);
	this->product=NULL;*/
	free(this);
	/*printf("\n pointeur de date : %d", (int)this->last_wash_date);
	if(this->next_dispenser->last_wash_date != NULL){
		Date_free(this->next_dispenser->last_wash_date);
		printf("\n détruire date");
	}
	printf("\n pointeur de produit : %d", (int)this->product);
	if(this->next_dispenser->product != NULL){
		Product_destroy(this->next_dispenser->product);
		printf("\n détruire produit");
	}*/

	//this->product=NULL;
	//this->last_wash_date=NULL;
	//this->message=NULL;
	//printf("\n \n \n --------- \n id : %d \n filling : %d \n battery : %d \n product size : %d \n date : %d  ",(int)this->next_dispenser->id, (int)this->next_dispenser->filling, (int)this->next_dispenser->battery, this->next_dispenser->product->size, this->next_dispenser->last_wash_date->current_day);
	//free(this);
	//printf("------------------------\nafter freeing the dispenser");
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

Product* Dispenser_get_produit(Dispenser* this) {
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
	printf("\n avant l'assert");
	assert(this != NULL);
	printf("\n avant le if");
	//printf("\n le produit pas encore set : %d, %d", (int)this->product->name, this->product->size);
	if(this->product != NULL) {
		printf("\n je passe dans le if ? ");
		Product_destroy(this->product);
		printf("\n ai-je détruit le produit ? ");
	}
	printf("\n après le if");
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
		Date_free(this->last_wash_date);
	}
	this->last_wash_date = Date_set_current_date();
}

bool Dispenser_is_last_dispenser(Dispenser* this) {
	return (this->next_dispenser == NULL);
}
