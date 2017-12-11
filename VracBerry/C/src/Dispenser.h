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

#define MESSAGE_TO_SEND_SIZE (20)
#define DATE_SIZE (10)
#define MQ_DISPENSER_MANAGER_MSG_SIZE (2048)
#define MAX_COUNT_INVALID_MESSAGE (5)
#define MAX_COUNT_LOST_MESSAGE (5)

typedef uint8_t Battery;
typedef uint8_t Filling;
typedef uint8_t Dispenser_Id;
typedef uint8_t Invalid_count;
typedef uint8_t Lost_count;

typedef enum DispenserState_t {
	S_INIT = 0,
	S_NOP,
	S_WAIT_MESSAGE,
	S_RUN,
	S_CHECK_CPT_TIME_SINCE_LAST_MESSAGE,
	S_LOST,
	S_CHECK_DATA,
	S_CHECK_CPT_INVALID_MESSAGE,
	S_DEFICIENT,
	S_CHECK_MESSAGE,
	S_DEATH,
	S_NBR_STATE
} DispenserState;

typedef enum {
	E_UNKNOWN = 0, // TODO
	E_ALREADY_KNOWN, // TODO
	E_RECEIVED_MESSAGE,
	E_TIMER, // TODO
	E_INF_CPT_TIME_SINCE_LAST_MESSAGE,
	E_SUP_CPT_TIME_SINCE_LAST_MESSAGE,
	E_INVALID_DATA,
	E_MAX_CPT_INVALID_MESSAGE,
	E_INF_CPT_INVALID_MESSAGE,
	E_VALID_DATA,
	E_ASK_UPDATE,
	E_ASK_DETAILED,
	E_NO_MESSAGE,
	E_MESSAGE,
	E_DESTROY, // TODO
	E_NBR_EVENT
} DispenserEvent;

typedef enum DispenserAction {
	A_SEND_INFOS = 0,
	A_ALREADY_KNOWN,
	A_INIT_COUNTERS,
	A_RECEIVED_MESSAGE,
	A_INC_CPT_TIME_SINCE_LAST_MESSAGE,
	A_RESET_TIMER,
	A_SUP_CPT_TIME_SINCE_LAST_MESSAGE,
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
	DispenserEvent event;
	Dispenser_Id id;
	Battery battery;
	Filling filling;
} DispenserMqMsg;

typedef union
{
	char buffer[MQ_DISPENSER_MANAGER_MSG_SIZE];
	DispenserMqMsg data;
} DispenserMqMsgAdapter;

typedef struct Dispenser_t Dispenser;

struct Dispenser_t {
	Dispenser_Id id;
	Filling filling;
	Battery battery;
	DispenserState state;
	Invalid_count invalid_count;
	Lost_count lost_count;
	Product* product;
	Date* last_wash_date;
	Dispenser* next_dispenser;
	MessageToSend* message;
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

extern void Dispenser_destroy(Dispenser*);

extern int Dispenser_get_day(Dispenser*);

extern int Dispenser_get_year(Dispenser*);

extern char* Dispenser_get_product_name(Dispenser*);

extern uint8_t Dispenser_get_product_size(Dispenser*);

extern MessageToSend* Dispenser_get_message(Dispenser*);

extern void Dispenser_printf(Dispenser*, char*);

extern void Dispenser_run(Dispenser*, DispenserMqMsg);


#endif /* SRC_DISPENSER_H_ */
