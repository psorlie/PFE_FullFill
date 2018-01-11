/*
 * DispenserManager.h
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_DISPENSERMANAGER_H_
#define SRC_DISPENSERMANAGER_H_


//TODO
#define BACKUP_FILE "/jesaispas/backup.txt"

#include "Network_Configuration.h"
#include "Dispenser.h"

/**
 * @def type for the head of the list
 */
typedef struct Dispenser_list_t Dispenser_list;


/**
 * @brief function to get the first dispenser of the list
 *
 * @retval Dispenser* the first dispenser.
 */
extern Dispenser* DispenserManager_get_list();

/**
 * @brief function to initialize the list (malloc of the head, ...)
 *
 * the list hasnt been initialized
 *
 * @retval void
 */
extern void DispenserManager_init();

extern Dispenser* DispenserManager_add_dispenser(Dispenser_Id, char*, Battery, Filling);

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

extern void DispenserManager_send_warning_lost_dispenser(Dispenser_Id);

extern void DispenserManager_prepare_destroy_dispenser(Dispenser_Id);

/**
 * @brief function called from the translator that had a dispenser with the data he sent
 *
 * @param[in] the id of the new dispenser
 *
 * @param[in] the battery value it sent
 *
 * @param[in] the filling value it sent
 *
 * @retval Dispenser* of the new dispenser
 */
extern Dispenser* DispenserManager_add_new_detected_dispenser(Dispenser_Id, Battery, Filling);

/**
 * @brief function called when the dispenser is in run state and an user ask the product name
 *
 * @pre the dispenser is in run state ?
 *
 * @param[in] Dispenser* of the asked dispenser
 */
extern void DispenserManager_ask_product_name(Dispenser*);

/**
 * @brief function to tell to a dispenser that he is broken
 *
 * @param[in] id of the dispenser
 */
extern void DispenserManager_tell_dispenser_broken(Dispenser_Id);

/**
 * @brief function to tell to a dispenser he has to repeat his last message.
 *
 * @param[in] id of the dispenser
 */
extern void DispenserManager_tell_dispenser_repeat(Dispenser_Id);


/**
 * @brief function called to save the data of a dispenser in the backup file
 *
 * @param[in] Dispenser* of the to-be saved dispenser
 */
extern void DispenserManager_save_in_backup(Dispenser*);

/**
 * @brief function called every morning to know if there is dirty dispenser
 *
 * @param[in] the maximum days since last cleaning
 */
extern void DispenserManager_check_dispenser_is_dirty(int);

/**
 * @brief function called to generate for every dispenser that the day is over
 */
extern void DispenserManager_end_of_the_day();

/**
 * @brief function called to generate for every dispenser that the night is over
 */
extern void DispenserManager_morning();



#endif /* SRC_DISPENSERMANAGER_H_ */
