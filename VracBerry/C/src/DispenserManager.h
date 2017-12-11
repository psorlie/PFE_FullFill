/*
 * DispenserManager.h
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_DISPENSERMANAGER_H_
#define SRC_DISPENSERMANAGER_H_

#define MQ_DISPENSER_MANAGER_NAME "/mq_dispenser_manager"
#define MQ_DISPENSER_MANAGER_MSG_SIZE (2048)
#define MQ_DISPENSER_MANAGER_MSG_COUNT (10)

#include "Dispenser.h"

typedef struct Dispenser_list_t Dispenser_list;

extern Dispenser* DispenserManager_get_list();

extern void DispenserManager_init();

extern void DispenserManager_add_dispenser(Dispenser_Id, char*, Battery, Filling);

extern void DispenserManager_free_dispenser(Dispenser_Id);

extern Dispenser* DispenserManager_find_dispenser(Dispenser_Id);

extern void DispenserManager_update_battery(Dispenser_Id, Battery);

extern void DispenserManager_update_filling(Dispenser_Id, Filling);

extern void DispenserManager_update_product(Dispenser_Id, char*);

extern void DispenserManager_update_last_wash_date(Dispenser_Id);

extern void DispenserManager_add_message(MessageToSend*, Dispenser_Id);

extern uint8_t DispenserManager_get_battery(Dispenser_Id);

extern uint8_t DispenserManager_get_filling(Dispenser_Id);

extern char* DispenserManager_get_product(Dispenser_Id);

extern int DispenserManager_get_cleaning_day(Dispenser_Id);

extern int DispenserManager_get_cleaning_year(Dispenser_Id);

extern MessageToSend* DispenserManager_get_message(Dispenser_Id);

extern void DispenserManager_printf(char*);

extern void DispenserManager_send_update(Dispenser*);

extern void DispenserManager_send_detailed_dispenser(Dispenser*);

extern void DispenserManager_check_message(Dispenser*);

extern void DispenserManager_check_conformity_data(DispenserMqMsg);

extern void DispenserManager_compromised_dispenser_event(Dispenser*);

extern void DispenserManager_not_compromised_dispenser_event(Dispenser*);

extern void DispenserManager_lost_dispenser_event(Dispenser*);

extern void DispenserManager_not_lost_dispenser_event(Dispenser*);

extern void DispenserManager_received_message(Dispenser_Id, Battery, Filling);

extern void DispenserManager_send_message_to_dispenser(Dispenser_Id, MessageToSend*);

extern void DispenserManager_send_warning_broken_dispenser(Dispenser_Id);

extern void DispenserManager_prepare_destroy_dispenser(Dispenser_Id);

#endif /* SRC_DISPENSERMANAGER_H_ */
