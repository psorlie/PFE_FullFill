/*
 * DispenserManager.h
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_DISPENSERMANAGER_H_
#define SRC_DISPENSERMANAGER_H_

#define MQ_DISPENSER_MANAGER_NAME           "/mq_dispenser_manager"
#define MQ_DISPENSER_MANAGER_MSG_SIZE (2048)
#define MQ_DISPENSER_MANAGER_MSG_COUNT (10)

//TODO
/**
 * @def filepath to backup file - TBD
 */
#define BACKUP_FILE "/jesaispas/backup.txt"

//#include "Network_Configuration.h"
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


/*************************************** Get ***********************************************/


extern uint8_t DispenserManager_get_battery(Dispenser_Id);

extern uint8_t DispenserManager_get_filling(Dispenser_Id);

extern char* DispenserManager_get_product(Dispenser_Id);

extern int DispenserManager_get_cleaning_day(Dispenser_Id);

extern int DispenserManager_get_cleaning_month(Dispenser_Id);

extern int DispenserManager_get_cleaning_year(Dispenser_Id);


/*************************************** Affichage ********************************************/

/**
 * @brief function to call when you want to print the whole Dispenser list
 * use 'v' arg if you want detailed information about the dispenser, anything else will
 * just print basic information
 *
 * @param[in] char*, pointer to the argument for the function (v for verbose or anything else)
 *
 */
extern void DispenserManager_printf(char*);


/********************************** Liste chaînée de dispenser *******************************************/

/**
 * @brief This function is used by the manager thread, to create a new dispenser if he
 * doesnt already exist in the chained list (search by id), add it in the list and return it
 *
 * @param[in] Dispenser_Id, id of the new dispenser
 *
 * @param[in] char*, future product name in the dispenser
 *
 * @param[in] Battery, battery level of the new dispenser
 *
 * @param[in] Filling, filling level of the new dispenser
 */
extern Dispenser* DispenserManager_add_dispenser(Dispenser_Id, char*, Battery, Filling);

/**
 * @brief This function will find the specified dispenser in the list, free it and
 * repair the list.
 *
 * @param[in] Dispenser_Id, id of the to-be free dispenser
 */
extern void DispenserManager_free_dispenser(Dispenser_Id);

/**
 * @brief This function is used to find a specified dispenser in the list and return it
 *
 * @param[in] Dispenser_Id, id of the dispenser we want to find
 *
 * @return Dispenser*, pointer to the specified dispenser
 */
extern Dispenser* DispenserManager_find_dispenser(Dispenser_Id);


/******************************************* Initialisation ******************************************/


/**
 * @brief function to initialize the list (malloc of the head, ...)
 *
 * the list hasnt been initialized
 *
 * @retval void
 */
extern void DispenserManager_init();


/******************************************* MAE ***************************************************/

/**
 * @brief This function is called from a Dispenser SM, it generate a compromised event for setting this
 * dispenser in broken mode if specified in the SM.
 *
 * @param[in] Dispenser*, pointer to the dispenser
 */
extern void DispenserManager_compromised_dispenser_event(Dispenser*);

/**
 * @brief This function is called from a Dispenser SM, it generate a non-compromised event for setting
 * this dispenser back to normal mode and asking for a repeat request to the physical dispenser.
 *
 * @param[in] Dispenser*, pointer to the dispenser
 */
extern void DispenserManager_not_compromised_dispenser_event(Dispenser*);

/**
 * @brief This function is called from a Dispenser SM, it generate a lost event for setting this
 * dispenser in lost mode.
 *
 * @param[in] Dispenser*, pointer to the dispenser
 */
extern void DispenserManager_lost_dispenser_event(Dispenser*);

/**
 * @brief This function is called from a Dispenser SM, it generates a 'not lost' event for setting
 * for setting the dispenser in normal mode.
 *
 * @param[in] Dispenser*, pointer to the dispenser
 */
extern void DispenserManager_not_lost_dispenser_event(Dispenser*);


/**************************************** Network config ************************************/

/**
 * @brief This function is used by a Dispenser who wants to be saved in the backup file. You should
 * think about creating a special module to manage backup-related issue (file/database, ...).
 *
 * @param[in] Dispenser*, pointer to the dispenser
 */
extern void DispenserManager_save_in_backup(Dispenser*);

/**
 * @brief This function is used at the beginning of a new day (and in the future probably
 *  for every new connection ?) to check if the delay since last cleaning for every dispenser
 *  is bigger than the one in the Network Configuration.
 *
 *  @param[in] int, the maximum number of days before a dispenser is dirty
 */
extern void DispenserManager_check_dispenser_is_dirty(int);

/**
 * @brief function called to generate for every dispenser that the day is over. It will create a 'end of day'
 * event for every dispenser in the list.
 */
extern void DispenserManager_end_of_the_day();

/**
 * @brief function called to generate for every dispenser that the night is over. It will create a
 * 'morning' event for every dispenser in the list.
 */
extern void DispenserManager_morning();

/**
 * @brief This function is used when the user want to change the delay between emission for the dispenser
 *
 * @Depreciated, we dont check periodically (every XX minutes), but every day in the v1. Could
 *  be used back again in v2.
 *
 *  @param[in] uint16_t, the number of minutes before we consider to have lost a dispenser packet.
 */
extern void DispenserManager_change_configuration(uint16_t);


/***************************************** Appli ****************************************/

/**
 * @brief This function is called after an user asked for the details of a specified dispenser. If the data
 * about it are accessible, we will send it back to the user.
 *
 * @param[in] Dispenser*, pointer to the asked dispenser
 */
extern void DispenserManager_send_detailed_dispenser(Dispenser*);

/**
 * @brief This function is called when a dispenser has received an update : we have to send this update
 * to every usere connected (at least on the generic page ? @To-be-specified ?
 *
 * @param[in] Dispenser* pointer to the updated dispenser.
 */
extern void DispenserManager_send_update(Dispenser*);

/**
 * @brief This function is called when a dispenser is broken : every connected user has to be alerted about it.
 * v2 : New user should also probably be alerted about it ? @To-be-specified ?
 *
 * @param[in] Dispenser_Id, id of the broken dispenser
 */
extern void DispenserManager_send_warning_broken_dispenser(Dispenser_Id);

/**
 * @brief This function is called when a dispenser is detected as lost (no message during the day), an alert
 * is send to every connected user to tell them. v2 : probably an alert to send to every new user as long it isn't repaired ?
 *
 * @param[in] Dispenser_Id, id of the lost dispenser
 */
extern void DispenserManager_send_warning_lost_dispenser(Dispenser_Id);

/**
 * @brief function called when the dispenser is in run state and an user ask the product name only when
 * the dispenser are accessible.
 *
 * @pre the dispenser is in run state ?
 *
 * @param[in] Dispenser* of the asked dispenser
 */
extern void DispenserManager_ask_product_name(Dispenser*);

/**
 * @brief This function is called when an user set a new product name for a dispenser. The event is then processed by
 * the specified dispenser.
 *
 * @param[in] Dispenser_Id, id of the dispenser whose product has changed
 *
 * @param[in] char*, the new name for the product inside the dispenser
 */
extern void DispenserManager_prepare_set_new_product_name(Dispenser_Id, char*);

/**
 * @brief This function is called when an user has washed a dispenser and wants to set to today the
 * washing date of the specified dispenser. Currently only work when the dispenser is in RUN_S.
 *
 * @param[in] Dispenser_Id, id of the clean dispenser
 */
extern void DispenserManager_prepare_set_current_date(Dispenser_Id);

/**
 * @brief This function is called when an user wants to have access to the detail of a specified dispenser.
 * v2 : Should think about sending an error message when we cant access the data ?
 *
 * @param[in] Dispenser_Id : id of the asked dispenser.
 */
extern void DispenserManager_ask_detailed_dispenser(Dispenser_Id);

/**
 * @brief This function is called when a new user is connected or when an user goes back to the
 * general windows of the website. We will send him all the data about all the dispenser we have.
 */
extern void DispenserManager_ask_all_update();


/**************************************** Zigbee *****************************************/

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
 * @brief This function is called after receiving data from a dispenser. We got the id of this dispenser,
 * and we will check if we have a message to send back to him.
 *
 * @param[in] Dispenser*, pointer to the specified dispenser
 */
extern void DispenserManager_check_message(Dispenser*);

/**
 * @brief This function is called when we have received data from a dispenser, we will check if they are conform
 * to the possible value.
 *
 * @param[in] DispenserMqMsg, the MQ message containing the to-be checked data.
 */
extern void DispenserManager_check_conformity_data(DispenserMqMsg);

/**
 * @brief This function is called when we receive data from a dispenser. We will stock the data and generate
 *  the right event for it.
 *
 *  @param[in] Dispenser_Id id of the dispenser who just sent it.
 *
 *  @param[in] Battery, battery level sent by the dispenser
 *
 *  @param[in] Filling, filling level sent by the dispenser
 */
extern void DispenserManager_received_message(Dispenser_Id, Battery, Filling);

/**
 * @brief This function is called when we have a message to send back to a specified dispenser
 *  after we received something from him.
 *
 * @param[in] Dispenser_Id, id of the dispenser to whom we are sending a message.
 *
 * @param[in] MessageToSend*, pointer to the message we are sending to the dispenser
 */
extern void DispenserManager_send_message_to_dispenser(Dispenser_Id, MessageToSend*);

/**
 * @brief This function is called when a dispenser is detected as broken, we send him a message telling him
 * we consider it as broken, so he has to stop himself.
 *
 * @param[in] Dispenser_Id, id of the broken dispenser
 */
extern void DispenserManager_tell_dispenser_broken(Dispenser_Id);

/**
 * @brief This function is called when a dispenser has send a non-conform data, we ask him to
 * measure again the filling and his battery and to send it again.
 *
 * @param[in] Dispenser_Id, id of the dispenser who has to repeat
 */
extern void DispenserManager_tell_dispenser_repeat(Dispenser_Id);

#endif /* SRC_DISPENSERMANAGER_H_ */
