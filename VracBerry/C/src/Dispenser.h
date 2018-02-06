/*
 * Dispenser.h
 *
 * This module is used to manipulate a dispenser and run his SM
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_DISPENSER_H_
#define SRC_DISPENSER_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <time.h>

#include "Date.h"
#include "Product.h"
#include "MessageToSend.h"
//#include "DispenserManager.h" // is used because the Dispenser will call DM function to generate event
							// for himself in case of pseudo-state transition

#define MESSAGE_TO_SEND_SIZE (20)
#define DATE_SIZE (10)
#define MAX_COUNT_INVALID_MESSAGE (5)
#define MAX_COUNT_LOST_MESSAGE (5)

#ifndef SIZE_MQ_MSG
#define SIZE_MQ_MSG
	#define MQ_DISPENSER_MANAGER_MSG_SIZE (2048)
#endif


/********************************************* Déclaration des types **************************************/


typedef uint8_t Battery;
typedef uint8_t Filling;
typedef uint8_t Dispenser_Id;
typedef uint8_t Invalid_count;
typedef bool Is_Lost;

typedef enum DispenserState_t {
	S_NOP = 0,
	S_RUN,
	S_LOST,
	S_CHECK_DATA,
	S_CHECK_HAS_EMITTED,
	S_CHECK_CPT_INVALID_MESSAGE,
	S_SLEEP,
	S_BROKEN,
	S_CHECK_MESSAGE,
	S_DEATH,
	S_NBR_STATE
} DispenserState;

typedef enum {
	E_SET_NEW_PRODUCT_NAME = 0,
	E_SET_CURRENT_DATE,
	E_RECEIVED_MESSAGE,
	E_END_OF_THE_DAY,
	E_HAS_EMITTED,
	E_HAS_NOT_EMITTED,
	E_MORNING,
	E_INVALID_DATA,
	E_MAX_CPT_INVALID_MESSAGE,
	E_INF_CPT_INVALID_MESSAGE,
	E_VALID_DATA,
	E_ASK_UPDATE,
	E_ASK_DETAILED,
	E_NO_MESSAGE,
	E_MESSAGE,
	E_DESTROY,
	E_NBR_EVENT
} DispenserEvent;

typedef enum DispenserAction {
	A_NOP = 0,
	A_MORNING,
	A_SET_NEW_PRODUCT_NAME,
	A_SET_CURRENT_DATE,
	A_RECEIVED_MESSAGE,
	A_LOST_ANSWER,
	A_BROKEN_ANSWER,
	A_END_OF_THE_DAY,
	A_SAVE_BACKUP,
	A_INVALID_DATA,
	A_MAX_CPT_INVALID_MESSAGE,
	A_ASK_NEW_MESSAGE,
	A_VALID_DATA,
	A_NO_MESSAGE_AND_UPDATE,
	A_SEND_DETAILED,
	A_SEND_UPDATE,
	A_MESSAGE_AND_UPDATE,
	A_DESTROY,
	A_NBR_ACTION
} DispenserAction;

typedef struct DispenserTransition_t {
	DispenserState futurState;
	DispenserAction futurAction;
} DispenserTransition;

typedef struct {
	Battery battery;
	Filling filling;
} BatteryAndFilling;

/**
 * @def the data transmitted can be the battery and the filling (from the dispenser) or a product name (from
 * an user).
 */
typedef union {
	BatteryAndFilling battery_and_filling;
	char product [PRODUCT_SIZE];
} DataTransmitted;

typedef struct {
	DispenserEvent event;
	Dispenser_Id id;
	DataTransmitted data_transmitted;
} DispenserMqMsg;

typedef union {
	char buffer[MQ_DISPENSER_MANAGER_MSG_SIZE];
	DispenserMqMsg data;
} DispenserMqMsgAdapter;

//typedef struct Dispenser_t Dispenser;

struct Dispenser_t;

typedef struct Dispenser_t Dispenser;
/**
 * @def A dispenser contain everything necessary about him, and a pointer to the next dispenser inside the list.
 */
struct Dispenser_t {
	Dispenser_Id id;
	Filling filling;
	Battery battery;
	DispenserState state;
	Invalid_count invalid_count;
	Is_Lost is_lost;
	Product* product;
	Date* last_wash_date;
	struct Dispenser_t* next_dispenser;
	MessageToSend* message;
};


/******************************************* get ******************************************/


extern Product* 		Dispenser_get_product(Dispenser*);

extern Battery 			Dispenser_get_battery(Dispenser*);

extern Dispenser_Id 	Dispenser_get_id(Dispenser*);

extern Filling 			Dispenser_get_filling(Dispenser*);

extern Date* 			Dispenser_get_last_wash_date(Dispenser*);

extern int 				Dispenser_get_day_of_month(Dispenser*);

extern int 				Dispenser_get_day_of_year(Dispenser*);

extern int 				Dispenser_get_month(Dispenser*);

extern int 				Dispenser_get_year(Dispenser*);

extern char* 			Dispenser_get_product_name(Dispenser*);

extern uint8_t 			Dispenser_get_product_size(Dispenser*);

extern MessageToSend* 	Dispenser_get_message(Dispenser*);


/******************************************* set ******************************************/


extern void 			Dispenser_set_product(Dispenser*, char*);

extern void				Dispenser_set_battery(Dispenser*, Battery);

extern void 			Dispenser_set_id(Dispenser*, Dispenser_Id);

extern void				Dispenser_set_filling(Dispenser*, Filling);

extern void 			Dispenser_set_current_date(Dispenser*);

extern void 			Dispenser_set_message(Dispenser*, MessageToSend*);

extern void 			Dispenser_set_specified_date(Dispenser*, int, int, int, int);


/******************************************* méthodes *******************************************/

/**
 * @brief This function will create a new dispenser (by malloc-ing the necessary memory size). and setting the
 * values in it and setting other to null/default value.
 *
 * @param[in] Dispenser_Id, id of the newly created dispenser
 *
 * @param[in] char*, name of the product inside the dispenser
 *
 * @param[in] Battery, battery level of the dispenser
 *
 * @param[in] Filling, filling level of the dispenser
 *
 * @return Dispenser*, pointer of the newly created dispenser
 */
extern Dispenser* 		Dispenser_create(Dispenser_Id, char*, Battery, Filling);

/**
 * @brief This function is used to print the information about the dispenser. Use the 'v' char as an attribute
 * for detailed information, anything else will just print basic data.
 *
 * @param[in] Dispenser*, pointer to the asked dispenser
 *
 * @param[in] char*, text for verbose (or not) data
 */
extern void 			Dispenser_printf(Dispenser*, char*);

/**
 * @brief This function is called with the right Dispenser* by the DispenserManager, it will get the right
 * action from the current state of the dispenser and the event in the MQ message to call the right function
 *
 * @param[in] Dispenser* pointer to the called dispenser for the action
 *
 * @param[in] DispenserMqMsg, MQ message containing the event and other data for the action
 */
extern void 			Dispenser_run(Dispenser*, DispenserMqMsg);

/**
 * @brief This funciton is called whenever the Manager get a message from a dispenser and cant find
 * the corresponding dispenser (same id), it will call this function to create a new dispenser in the list
 * It's basically the same as Dispenser_create, but without any product.
 *
 * @param[in] Dispenser_Id, id of the newly created dispenser
 *
 * @param[in] Battery, battery level of the dispenser
 *
 * @param[in] Filling, filling level of the dispenser
 *
 * @return Dispenser*, pointer to the new dispenser
 */
extern Dispenser* 		Dispenser_detected(Dispenser_Id, Battery, Filling);

/**
 * @brief This function is called when we want to free a dispenser from the list
 * it will call the destroy of all the object inside it, reset itself then free.
 *
 * @param[in] Dispenser*, pointer to the to-be destroyed dispenser.
 */
extern void 			Dispenser_destroy(Dispenser*);


#endif /* SRC_DISPENSER_H_ */
