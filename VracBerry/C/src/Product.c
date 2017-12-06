/**
 * @file Product.c
 *
 * @brief This module contain creation/destruction of a product and also getter/setter
 *
 *
 *
 * @version 1.0
 * @date 16/11/2017
 * @author Pierre-François SORLIE
 * @copyright BSD 2-clauses
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "Product.h"

/*************************************************
 ********** Static function declaration **********
 *************************************************/

/**
 * @brief static function to free the product
 *
 * @pre the product exist
 *
 * @param[in] Product* product pointer that will be deleted
 *
 * @retval void
 */
static void Product_free(Product*);

/**
 * @brief static function to set the size in function of the character in it
 * if it's bigger than PRODUCT_SIZE : it will be set as PRODUCT_SIZE and the character after won't be written
 *
 * @there is at least a char in the name
 *
 * @param[in] char* of the new product name
 *
 * @retval uint8_t : size of the name
 */
static uint8_t Product_set_size(char*);

static Product* Product_set_name(Product*, char*);

/*************************************************
 ********* Static function initialization ********
 *************************************************/

static uint8_t Product_set_size(char* new_product) {
	uint8_t ret = strlen(new_product);
	if(ret > PRODUCT_SIZE){
		return PRODUCT_SIZE;
	} else {
		return ret;
	}
}

static void Product_free(Product* this) {
	assert(this != NULL);
	free(this->name);
	this->name = (char*)NULL;
	free(this);
}

static Product* Product_set_name(Product* this, char* product) {
	Product* return_value = NULL;
	if(product != NULL){
		memset(this->name, '\0', (this->size + 1));
		snprintf(this->name, this->size + 1, "%s",product);
		return_value = this;
	}
	return return_value;
}

/*************************************************
 ********* public function initialization ********
 *************************************************/

Product* Product_set_product(char* new_product){
	assert(new_product != NULL);
	Product* this;
	this = (Product *)malloc(sizeof(Product));
	assert(this != NULL);
	this->size = Product_set_size(new_product);
	this->name = (char*)malloc(sizeof(this->size + 1));
	this = Product_set_name(this, new_product);
	return this;
}


void Product_destroy(Product* this) {
	if(this != NULL){
		Product_free(this);
	}
}

char* Product_get_name(Product* this) {
	assert(this != NULL);
	return this->name;
}

uint8_t Product_get_size(Product* this) {
	assert(this != NULL);
	return this->size;
}
