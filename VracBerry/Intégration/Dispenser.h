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
#include <stdbool.h>
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

typedef struct Dispenser_t Dispenser;

struct Dispenser_t {
	Dispenser_Id id;
	Filling filling;
	Battery battery;
	DispenserState state;
	Invalid_count invalid_count;
	Is_Lost is_lost;
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

extern Dispenser* Dispenser_detected(Dispenser_Id, Battery, Filling);

extern void Dispenser_set_specified_date(Dispenser*, int, int);


#endif /* SRC_DISPENSER_H_ */
