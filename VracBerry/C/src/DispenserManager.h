/*
 * DispenserManager.h
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_DISPENSERMANAGER_H_
#define SRC_DISPENSERMANAGER_H_

typedef struct Dispenser_list_t Dispenser_list;

extern void DispenserManager_add_dispenser(Dispenser_Id, char*, Battery, Filling);

extern void DispenserManager_free_dispenser(Dispenser_Id);

extern Dispenser* DispenserManager_find_dispenser(Dispenser_Id);

#endif /* SRC_DISPENSERMANAGER_H_ */
