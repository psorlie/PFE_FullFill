/*
 * Dispenser.h
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_DISPENSER_H_
#define SRC_DISPENSER_H_

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#include "Date.h"

#define MANAGER_MQ_NAME "/mq_dispatcher_manager"
#define PRODUCT_SIZE (20)
#define MESSAGE_TO_SEND_SIZE (20)
#define DATE_SIZE (10)

typedef uint8_t Battery;
typedef uint8_t Filling;
typedef uint8_t Dispenser_Id;


typedef struct Dispenser_t Dispenser;

typedef struct {
	char[PRODUCT_SIZE];
}Product;

typedef struct {
	char[MESSAGE_TO_SEND_SIZE];
}Message_to_send;


extern Dispenser * Dispenser_create(Dispenser_Id, Product, Battery, Filling);

extern Product Dispenser_get_product(Dispenser*);

extern Battery Dispenser_get_battery(Dispenser*);

extern Dispenser_Id Dispenser_get_id(Dispenser*);

extern Filling Dispenser_get_filling(Dispenser*);

extern void Dispenser_set_product(Dispenser*, Product);

extern void Dispenser_set_battery(Dispenser*, Battery);

extern void Dispenser_set_id(Dispenser*, Dispenser_Id);

extern void Dispenser_set_filling(Dispenser*, Filling);

#endif /* SRC_DISPENSER_H_ */
