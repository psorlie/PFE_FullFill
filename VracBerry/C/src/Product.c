/*
 * Product.c
 *
 *  Created on: 14 nov. 2017
 *      Author: sorlie
 */

#include <stdlib.h>
#include <string.h>

#include "Product.h"

static void Product_free(Product*);


bool Product_setProduct(Product* this, char* new_product){
	if (this != NULL){
		Product_free(this);
	}
	this = (Product *)malloc(sizeof(Product));
	strcpy(this->product , new_product);
	bool result;
	if (strcmp(this->product, new_product)) {
		result = true;
	} else {
		result = false;
	}
	return result;
}

void Product_destroy(Product* this) {
	Product_free(this);
}

static void Product_free(Product* this) {
	free(this);
}
