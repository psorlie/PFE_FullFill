/*
 * Product.h
 *
 *  Created on: 14 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_PRODUCT_H_
#define SRC_PRODUCT_H_

#include <stdbool.h>
#include <stdint.h>

#define PRODUCT_SIZE (20)

typedef struct {
	char* name;
	uint8_t size;
}Product;

#endif /* SRC_PRODUCT_H_ */

extern void Product_destroy(Product*);
extern Product* Product_set_product(char*);

extern char* Product_get_name(Product*);

extern uint8_t Product_get_size(Product*);
