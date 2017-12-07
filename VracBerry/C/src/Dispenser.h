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
#define MQ_MSG_SIZE (2048)

typedef uint8_t Battery;
typedef uint8_t Filling;
typedef uint8_t Dispenser_Id;

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
	E_UNKNOWN = 0,
	E_ALREADY_KNOWN,
	E_RECEIVED_MESSAGE,
	E_TIMER,
	E_INF_CPT_TIME_SINCE_LAST_MESSAGE,
	E_SUP_CPT_TIME_SINCE_LAST_MESSAGE,
	E_NEW_MESSAGE,
	E_INVALID_DATA,
	E_MAX_CPT_INVALID_MESSAGE,
	E_INF_CPT_INVALID_MESSAGE,
	E_VALID_DATA,
	E_NO_MESSAGE,
	E_MESSAGE,
	E_DESTROY,
	E_NBR_EVENT
} DispenserEvent;

typedef struct DispenserAction_t DispenserAction;

typedef struct DispenserTransition_t {
	DispenserState futurState;
	DispenserAction futurAction;
} DispenserTransition;

typedef struct {
	DispenserEvent event;
	Dispenser_Id id;
} DispenserMqMsg;

typedef union
{
	char buffer[MQ_MSG_SIZE];
	DispenserMqMsg data;
} MqMsgAdapter;

typedef struct Dispenser_t Dispenser;

struct Dispenser_t {
	Dispenser_Id id;
	Filling filling;
	Battery battery;
	DispenserState state;
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

#endif /* SRC_DISPENSER_H_ */
