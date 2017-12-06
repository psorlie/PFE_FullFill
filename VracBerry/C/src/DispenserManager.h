/*
 * DispenserManager.h
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_DISPENSERMANAGER_H_
#define SRC_DISPENSERMANAGER_H_

#include "Dispenser.h"

typedef struct Dispenser_list_t Dispenser_list;

extern Dispenser* DispenserManager_get_list();

extern void DispenserManager_add_dispenser(Dispenser_Id, char*, Battery, Filling);

extern void DispenserManager_free_dispenser(Dispenser_Id);

extern Dispenser* DispenserManager_find_dispenser(Dispenser_Id);

extern void DispenserManager_update_battery(Dispenser_Id, Battery);

extern void DispenserManager_update_filling(Dispenser_Id, Filling);

extern void DispenserManager_update_product(Dispenser_Id, char*);

extern void DispenserManager_update_last_wash_date(Dispenser_Id);

extern void DispenserManager_add_message(MessageToSend* Dispenser_Id);

extern uint8_t DispenserManager_get_battery(Dispenser_Id);

extern uint8_t DispenserManager_get_filling(Dispenser_Id);

extern char* DispenserManager_get_product(Dispenser_Id);

extern int DispenserManager_get_cleaning_day(Dispenser_Id);

extern int DispenserManager_get_cleaning_year(Dispenser_Id);

extern MessageToSend* DispenserManager_get_message(Dispenser_Id);

extern void DispenserManager_printf(char*);

#endif /* SRC_DISPENSERMANAGER_H_ */
