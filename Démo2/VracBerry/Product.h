/**
 * @file Product.h
 *
 * @brief This header contain the basic information
 * about the structure of a product and public function declaration
 *
 *
 *
 * @version 1.0
 * @date 14/11/2017
 * @author Pierre-François SORLIE
 * @copyright BSD 2-clauses
 */

#ifndef SRC_PRODUCT_H_
#define SRC_PRODUCT_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @def PRODUCT_SIZE : maximum size of the name of a product
 */
#define PRODUCT_SIZE (20)

#define DEFAULT_PRODUCT_NAME "XXXXX"

#define PRODUCT_REPEAT "repeat"

#define PRODUCT_BROKEN "broken"

/**
 * @def Product struct definition and what is inside
 */
typedef struct {
	char* name;
	uint8_t size;
}Product;

/**
 * @brief function to destroy the product : reset name and size
 *
 * @pre the product exist
 *
 * @param[in] Product* product pointer that will be deleted
 *
 * @retval void
 */
extern void Product_destroy(Product*);

/**
 * @brief Product constructor
 *
 * @pre the name of the product isn't empty
 *
 * @param[in] char* of the name
 *
 * @retval Product* of the the new product
 */
extern Product* Product_set_product(char*);

/**
 * @brief name getter of a product
 *
 * @pre the product exist
 *
 * @param[in] Product* product pointer whose name you want
 *
 * @retval char* of the name
 */
extern char* Product_get_name(Product*);

/**
 * @brief size getter of a product
 *
 * @pre the product exist
 *
 * @param[in] Product* product pointer whose size you want
 *
 * @retval uint8_t size of the product
 */
extern uint8_t Product_get_size(Product*);

#endif /* SRC_PRODUCT_H_ */


