/*
 * Product.c
 *
 *  Created on: 14 nov. 2017
 *      Author: sorlie
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "Product.h"

static void Product_free(Product*);

static uint8_t Product_set_size(char*);

static uint8_t Product_set_size(char* new_product) {
	uint8_t ret = strlen(new_product);
	return ret;
}


Product* Product_set_product(char* new_product){
	Product* this;
	this = (Product *)malloc(sizeof(Product));
	this->size = Product_set_size(new_product);
	this->name = (char*)malloc(sizeof(this->size));
	if(this->name == NULL){
		printf("mauvais");
	}
	memset(this->name, '\0', this->size);
	strcpy(this->name, new_product);
	return this;
}

void Product_destroy(Product* this) {
	Product_free(this);
}

static void Product_free(Product* this) {
	free(this);
}

char* Product_get_name(Product* this) {
	assert(this != NULL);
	return this->name;
}

uint8_t Product_get_size(Product* this) {
	assert(this != NULL);
	return this->size;
}
