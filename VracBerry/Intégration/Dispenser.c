/*
 * Dispenser.c
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "Dispenser.h"
#include "DispenserManager.h"

static DispenserTransition g_stateMachine[S_NBR_STATE][E_NBR_EVENT] = {
		[S_RUN][E_SET_NEW_PRODUCT_NAME] =
		{S_RUN, A_SET_NEW_PRODUCT_NAME},
		[S_RUN][E_SET_CURRENT_DATE] =
		{S_RUN, A_SET_CURRENT_DATE},
		[S_RUN][E_RECEIVED_MESSAGE] =
		{S_CHECK_DATA, A_RECEIVED_MESSAGE},
		[S_RUN][E_END_OF_THE_DAY] =
		{S_CHECK_HAS_EMITTED, A_END_OF_THE_DAY},
		[S_RUN][E_DESTROY] =
		{S_DEATH, A_DESTROY},
		[S_RUN][E_ASK_UPDATE] =
		{S_RUN, A_SEND_UPDATE},
		[S_RUN][E_ASK_DETAILED] =
		{S_RUN, A_SEND_DETAILED},


		[S_CHECK_HAS_EMITTED][E_HAS_EMITTED] =
		{S_SLEEP, A_SAVE_BACKUP},
		[S_CHECK_HAS_EMITTED][E_HAS_NOT_EMITTED] =
		{S_LOST, A_SAVE_BACKUP},

		[S_LOST][E_RECEIVED_MESSAGE] =
		{S_CHECK_DATA, A_RECEIVED_MESSAGE},
		[S_LOST][E_DESTROY] =
		{S_DEATH, A_DESTROY},
		[S_LOST][E_ASK_UPDATE] =
		{S_LOST, A_LOST_ANSWER},
		[S_LOST][E_ASK_DETAILED] =
		{S_LOST, A_LOST_ANSWER},


		[S_SLEEP][E_MORNING] =
		{S_RUN, A_MORNING},
		[S_SLEEP][E_ASK_UPDATE] =
		{S_SLEEP, A_NOP},
		[S_SLEEP][E_ASK_DETAILED] =
		{S_SLEEP, A_NOP},

		[S_CHECK_DATA][E_VALID_DATA] =
		{S_CHECK_MESSAGE, A_VALID_DATA},
		[S_CHECK_DATA][E_INVALID_DATA] =
		{S_CHECK_CPT_INVALID_MESSAGE, A_INVALID_DATA},

		[S_CHECK_CPT_INVALID_MESSAGE][E_INF_CPT_INVALID_MESSAGE] =
		{S_RUN, A_ASK_NEW_MESSAGE},
		[S_CHECK_CPT_INVALID_MESSAGE][E_MAX_CPT_INVALID_MESSAGE] =
		{S_BROKEN, A_MAX_CPT_INVALID_MESSAGE},

		[S_BROKEN][E_DESTROY] =
		{S_DEATH, A_DESTROY},
		[S_BROKEN][E_ASK_UPDATE] =
		{S_BROKEN, A_BROKEN_ANSWER},
		[S_BROKEN][E_ASK_DETAILED] =
		{S_BROKEN, A_BROKEN_ANSWER},

		[S_CHECK_MESSAGE][E_NO_MESSAGE] =
		{S_RUN, A_NO_MESSAGE_AND_UPDATE},
		[S_CHECK_MESSAGE][E_MESSAGE] =
		{S_RUN, A_MESSAGE_AND_UPDATE}
};

static void Dispenser_free(Dispenser*);

static void Dispenser_performAction(Dispenser*, DispenserAction, DispenserMqMsg);

static void Dispenser_send_ack(Dispenser*);

static void Dispenser_send_message(Dispenser*);

static MessageToSend* Dispenser_get_acknowledge_message();

static Invalid_count Dispenser_get_invalid_count(Dispenser*);

static void Dispenser_set_invalid_count(Dispenser*, Invalid_count);

static void Dispenser_reset_invalid_count(Dispenser*);

static void Dispenser_increment_invalid_count(Dispenser*);

static bool Dispenser_has_reached_max_invalid_message(Dispenser*);

static void Dispenser_ask_new_message(Dispenser*);

static MessageToSend* Dispenser_get_repeat_message();

static void Dispenser_save_data(Dispenser*, DispenserMqMsg);

static void Dispenser_check_invalid_message_count(Dispenser*);

static void Dispenser_check_has_emitted(Dispenser*);

static Is_Lost Dispenser_is_lost(Dispenser*);

static void Dispenser_reset_lost(Dispenser*);

static void Dispenser_check_is_dirty(Dispenser*);

static void Dispenser_has_received_a_message(Dispenser*);


void Dispenser_run(Dispenser* this, DispenserMqMsg message) {
	DispenserAction action;
	char bid [50];
	sprintf(bid, "state %d event %d", this->state, message.event);
	perror(bid);
	if(g_stateMachine[this->state][message.event].futurState == S_NOP){
		perror("[Dispenser] - Perte d'évènement");
	} else {
		action = g_stateMachine[this->state][message.event].futurAction;
		Dispenser_performAction(this, action, message);
		this->state = g_stateMachine[this->state][message.event].futurState;
	}
}

static void Dispenser_performAction(Dispenser* this, DispenserAction action, DispenserMqMsg message) {
	switch(action) {
	case A_SET_NEW_PRODUCT_NAME:
		Dispenser_set_product(this, message.data_transmitted.product);
		DispenserManager_send_detailed_dispenser(this);
		break;
	case A_SET_CURRENT_DATE:
		Dispenser_set_current_date(this);
		DispenserManager_send_detailed_dispenser(this);
		break;
	case A_RECEIVED_MESSAGE:
		Dispenser_has_received_a_message(this);
		DispenserManager_check_conformity_data(message);
		break;
	case A_BROKEN_ANSWER:
		DispenserManager_send_warning_broken_dispenser(this->id);
		break;
	case A_MORNING:
		Dispenser_check_is_dirty(this);
		Dispenser_reset_lost(this);
		break;
	case A_LOST_ANSWER:
		DispenserManager_send_warning_lost_dispenser(this->id);
		break;
	case A_SAVE_BACKUP:
		DispenserManager_save_in_backup(this);
		break;
	case A_END_OF_THE_DAY:
		Dispenser_check_has_emitted(this);
		break;
	case A_INVALID_DATA:
		Dispenser_increment_invalid_count(this);
		Dispenser_ask_new_message(this);
		Dispenser_check_invalid_message_count(this);
		break;
	case A_MAX_CPT_INVALID_MESSAGE:
		DispenserManager_tell_dispenser_broken(this->id);
		DispenserManager_send_warning_broken_dispenser(this->id);
		DispenserManager_save_in_backup(this);
		break;
	case A_ASK_NEW_MESSAGE:
		DispenserManager_tell_dispenser_repeat(this->id);
		break;
	case A_VALID_DATA:
		Dispenser_reset_invalid_count(this);
		Dispenser_save_data(this, message);
		DispenserManager_check_message(this);
		break;
	case A_DESTROY:
		DispenserManager_free_dispenser(this->id);
		break;
	case A_SEND_UPDATE:
		Dispenser_check_is_dirty(this);
		DispenserManager_send_update(this);
		break;
	case A_SEND_DETAILED:
		DispenserManager_send_detailed_dispenser(this);
		break;
	case A_NO_MESSAGE_AND_UPDATE:
		Dispenser_send_ack(this);
		DispenserManager_send_update(this);
		break;
	case A_MESSAGE_AND_UPDATE:
		Dispenser_send_message(this);
		DispenserManager_send_update(this);
		break;
	default:
		perror("NOT COMPLETED STATE MACHINE");
		break;
	}
}

Dispenser* Dispenser_create(Dispenser_Id id, char* product, Battery battery, Filling filling) {
	Dispenser * this;
	this = (Dispenser *) malloc (sizeof(Dispenser));
	this->battery = 0;
	this->filling = 0;
	this->id = 0;
	this->invalid_count = 0;
	this->is_lost = true;
	this->last_wash_date = NULL;
	this->product = NULL;
	this->next_dispenser = NULL;
	this->message = NULL;
	this->state = S_RUN;
	Dispenser_set_id(this, id);
	Dispenser_set_filling(this, filling);
	Dispenser_set_battery(this, battery);
	Dispenser_set_product(this, product);
	Dispenser_set_current_date(this);
	return this;
}

Dispenser* Dispenser_detected(Dispenser_Id id, Battery battery, Filling filling) {
	Dispenser * this;
	this = (Dispenser *) malloc (sizeof(Dispenser));
	this->battery = 0;
	this->filling = 0;
	this->id = 0;
	this->invalid_count = 0;
	this->is_lost = true;
	this->last_wash_date = NULL;
	this->product = NULL;
	this->next_dispenser = NULL;
	this->message = NULL;
	this->state = S_RUN;
	Dispenser_set_id(this, id);
	Dispenser_set_filling(this, filling);
	Dispenser_set_battery(this, battery);
	Dispenser_set_current_date(this);
	return this;
}

void Dispenser_destroy(Dispenser* this) {
	if(this != NULL) {
		Dispenser_free(this);
	}
}

static void Dispenser_free(Dispenser* this) {
	this->id = 0;
	this->battery = 0;
	this->filling = 0;
	this->invalid_count = 0;
	this->is_lost = false;
	this->next_dispenser = NULL;
	Product_destroy(this->product);
	this->product = NULL;
	Date_destroy(this->last_wash_date);
	this->last_wash_date = NULL;
	MessageToSend_destroy(this->message);
	this->message = NULL;
	free(this);
}

/* Getter */

char* Dispenser_get_product_name(Dispenser* this) {
	assert(this != NULL);
	char* ptr_to_name = Product_get_name(this->product);
	return ptr_to_name;
}

uint8_t Dispenser_get_product_size(Dispenser* this) {
	assert(this != NULL);
	uint8_t size = Product_get_size(this->product);
	return size;
}

Product* Dispenser_get_product(Dispenser* this) {
	assert(this != NULL);
	return this->product;
}

Battery Dispenser_get_battery(Dispenser* this) {
	assert(this != NULL);
	return this->battery;
}

Dispenser_Id Dispenser_get_id(Dispenser* this) {
	assert(this != NULL);

	return this->id;
}

Filling Dispenser_get_filling(Dispenser* this) {
	assert(this != NULL);

	return this->filling;
}

MessageToSend* Dispenser_get_message(Dispenser* this) {
	assert(this != NULL);

	bool existing_message = MessageToSend_exist(this->message);
	if(!existing_message) {
		return NULL;
	}
	return this->message;
}

Date* Dispenser_get_last_wash_date(Dispenser* this) {
	assert(this != NULL);

	return this->last_wash_date;
}

int Dispenser_get_year(Dispenser* this) {
	assert(this != NULL);
	int ret = Date_get_year(this->last_wash_date);
	assert(ret > 1900);
	return ret;
}

int Dispenser_get_month(Dispenser* this) {
	assert(this != NULL);
	int ret = Date_get_month(this->last_wash_date);
	assert(ret >= 0 && ret <= 12);
	return ret;
}

int Dispenser_get_day_of_month(Dispenser* this) {
	assert(this != NULL);
	int ret = Date_get_day_of_month(this->last_wash_date);
	assert(ret >= 0 && ret <= 31);
	return ret;
}

int Dispenser_get_day_of_year(Dispenser* this) {
	assert(this != NULL);
	int ret = Date_get_day_of_year(this->last_wash_date);
	assert(ret >= 0 && ret <= 366);
	return ret;
}

/* Setter */

void Dispenser_set_product(Dispenser* this, char* product) {
	assert(this != NULL);
	if(this->product != NULL) {
		Product_destroy(this->product);
	}
	this->product = Product_set_product(product);
}

void Dispenser_set_battery(Dispenser* this, Battery battery) {
	assert(this != NULL);

	this->battery = battery;
}

void Dispenser_set_id(Dispenser* this, Dispenser_Id id) {
	assert(this != NULL);

	this-> id = id;
}

void Dispenser_set_filling(Dispenser* this, Filling filling) {
	assert(this != NULL);

	this->filling = filling;
}

void Dispenser_set_message(Dispenser* this, MessageToSend* message) {
	assert(this != NULL);

	this->message = message;
}

void Dispenser_set_current_date(Dispenser* this) {
	assert(this != NULL);

	if(this->last_wash_date != NULL) {
		Date_destroy(this->last_wash_date);
	}
	this->last_wash_date = Date_set_current_date();
}

void Dispenser_set_specified_date(Dispenser* this, int day_of_month, int day_of_year, int month, int year) {
	if(this->last_wash_date != NULL) {
		Date_destroy(this->last_wash_date);
	}
	this->last_wash_date = Date_set_specified_date(day_of_month, day_of_year, month, year);
}

bool Dispenser_is_last_dispenser(Dispenser* this) {
	assert(this != NULL);
	return (this->next_dispenser == NULL);
}

void Dispenser_printf(Dispenser* this, char* arg) {
	assert(this != NULL);
	printf("\n---------------------------------------------------------------------------------\n");

	char* product = Dispenser_get_product_name(this);
	printf("Dispenser numéro %d : contient du %s, est rempli à %d %% et sa batterie est à %d %%\n", this->id, product, this->filling, this->battery);
	if(strcmp(arg, "v") == 0) {
		printf("Date dernier lavage : %d-%d-%d -- %d-%d\n", 
				this->last_wash_date->day_of_month, this->last_wash_date->month, this->last_wash_date->year, 
				this->last_wash_date->day_of_year, this->last_wash_date->year);
		if(this->message !=NULL){
			printf("message en queue pour le dispenser : %s\n", this->message->message);
		} else {
			printf("Pas de message en queue \n");
		}
	}
	printf("---------------------------------------------------------------------------------\n");

}

static void Dispenser_send_ack(Dispenser* this) {
	MessageToSend* message;
	Dispenser_Id id;
	message = Dispenser_get_acknowledge_message();
	id = Dispenser_get_id(this);
	DispenserManager_send_message_to_dispenser(id, message);
}

static void Dispenser_send_message(Dispenser* this) {
	MessageToSend* message;
	Dispenser_Id id;
	message = Dispenser_get_message(this);
	id = Dispenser_get_id(this);
	DispenserManager_send_message_to_dispenser(id, message);
}

static void Dispenser_ask_new_message(Dispenser* this) {
	MessageToSend* message;
	Dispenser_Id id;
	message = Dispenser_get_repeat_message();
	id = Dispenser_get_id(this);
	DispenserManager_send_message_to_dispenser(id, message);
}

static MessageToSend* Dispenser_get_repeat_message() {
	MessageToSend* message;
	message = MessageToSend_get_repeat_message();
	return message;
}


static MessageToSend* Dispenser_get_acknowledge_message() {
	MessageToSend* message;
	message = MessageToSend_get_acknowledge_message();
	return message;
}

static Invalid_count Dispenser_get_invalid_count(Dispenser* this) {
	return this->invalid_count;
}

static void Dispenser_set_invalid_count(Dispenser* this, Invalid_count invalid_count) {
	this->invalid_count = invalid_count;
}

static void Dispenser_reset_invalid_count(Dispenser* this) {
	Dispenser_set_invalid_count(this, 0);
}

static void Dispenser_increment_invalid_count(Dispenser* this) {
	Invalid_count invalid_count;
	invalid_count = Dispenser_get_invalid_count(this);
	invalid_count++;
	Dispenser_set_invalid_count(this, invalid_count);
}

static void Dispenser_check_invalid_message_count(Dispenser* this) {
	bool result;
	result = Dispenser_has_reached_max_invalid_message(this);
	if(result){
		DispenserManager_compromised_dispenser_event(this);
	} else {
		DispenserManager_not_compromised_dispenser_event(this);
	}
}

static bool Dispenser_has_reached_max_invalid_message(Dispenser* this) {
	bool result;
	result = Dispenser_get_invalid_count(this) < MAX_COUNT_INVALID_MESSAGE;
	return result;
}

static void Dispenser_save_data(Dispenser* this, DispenserMqMsg message) {
	Dispenser_set_battery(this, message.data_transmitted.battery_and_filling.battery);
	Dispenser_set_filling(this, message.data_transmitted.battery_and_filling.filling);
	DispenserManager_check_message(this);
}

static Is_Lost Dispenser_is_lost(Dispenser* this) {
	return this->is_lost;
}

static void Dispenser_reset_lost(Dispenser* this) {
	this->is_lost = true;
}

static void Dispenser_has_received_a_message(Dispenser* this) {
	this->is_lost = false;
}

static void Dispenser_check_has_emitted(Dispenser* this) {
	bool is_lost = (bool)Dispenser_is_lost(this);
	if(is_lost) {
		DispenserManager_lost_dispenser_event(this);
	} else {
		DispenserManager_not_lost_dispenser_event(this);
	}
}

static void Dispenser_check_is_dirty(Dispenser* this) {
	// TODO : à modifier
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	int day_diff = timeinfo->tm_yday - this->last_wash_date->day_of_year;
	int year_diff = timeinfo->tm_year - this->last_wash_date->year;

	int total_day_diff = year_diff*365 + day_diff;

	DispenserManager_check_dispenser_is_dirty(total_day_diff);
}
