/*
 * Product.h
 *
 *  Created on: 14 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_PRODUCT_H_
#define SRC_PRODUCT_H_

#include <stdbool.h>

#define PRODUCT_SIZE (20)

typedef struct {
	char* product;
}Product;

#endif /* SRC_PRODUCT_H_ */

extern void Product_destroy(Product*);
extern void Product_set_product(Product*, char*);
