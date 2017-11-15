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
#include "Product.h"
#include "MessageToSend.h"

#define MANAGER_MQ_NAME "/mq_dispatcher_manager"

#define MESSAGE_TO_SEND_SIZE (20)
#define DATE_SIZE (10)

typedef uint8_t Battery;
typedef uint8_t Filling;
typedef uint8_t Dispenser_Id;



typedef struct Dispenser_t Dispenser;

struct Dispenser_t {
	MessageToSend* message;
	Dispenser_Id id;
	Filling filling;
	Battery battery;
	Product* product;
	Date* last_wash_date;
	Dispenser* next_dispenser;
};

extern Dispenser* Dispenser_create(Dispenser_Id, char*, Battery, Filling);

extern Product* Dispenser_get_product(Dispenser*);

extern Battery Dispenser_get_battery(Dispenser*);

extern Dispenser_Id Dispenser_get_id(Dispenser*);

extern Filling Dispenser_get_filling(Dispenser*);

extern void Dispenser_set_product(Dispenser*, char*);

extern void Dispenser_set_battery(Dispenser*, Battery);

extern void Dispenser_set_id(Dispenser*, Dispenser_Id);

extern void Dispenser_set_filling(Dispenser*, Filling);

extern void Dispenser_set_current_date(Dispenser*);

extern void Dispenser_free(Dispenser*);

extern int Dispenser_get_day(Dispenser*);

extern int Dispenser_get_year(Dispenser*);

extern char* Dispenser_get_product_name(Dispenser*);

extern uint8_t Dispenser_get_product_size(Dispenser*);

#endif /* SRC_DISPENSER_H_ */
