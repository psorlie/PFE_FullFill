/*
 * DispenserManager.c
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <pthread.h>

#include "Application.h"
#include "DispenserManager.h"
#include "Translator.h"


struct Dispenser_list_t {
	Dispenser* first_dispenser;
};

static pthread_t dispenser_manager_thread;

static Dispenser_list* dispenser_list;

static Dispenser* DispenserManager_find_before_dispenser(Dispenser_Id);

static Dispenser_list* DispenserManager_initialisation();

static void DispenserManager_mq_send(DispenserMqMsg);

static DispenserMqMsg DispenserManager_mq_receive();

static void DispenserManager_ask_update(Dispenser*);

static void* DispenserManager_run();

static void DispenserManager_read_backup();

//TODO static bool DispenserManager_exist(Dispenser_Id);

static Dispenser_list* DispenserManager_initialisation() {
	assert(dispenser_list == NULL);

	Dispenser_list* list = malloc(sizeof(Dispenser_list));
	if(list == NULL){
		exit(0);
	}
	return list;
}

void DispenserManager_init() {
	struct mq_attr mail_queue_attributes;
		int check;
		mqd_t mail_queue;
		mail_queue_attributes.mq_maxmsg = MQ_DISPENSER_MANAGER_MSG_COUNT;
		mail_queue_attributes.mq_msgsize = MQ_DISPENSER_MANAGER_MSG_SIZE;
		mail_queue_attributes.mq_flags = 0;

		check = mq_unlink(MQ_DISPENSER_MANAGER_NAME);
		if(check != 0) {
			perror("[Postman] - MQ already exist ?\n");
		}

		mail_queue = mq_open(MQ_DISPENSER_MANAGER_NAME, O_CREAT, 0777, &mail_queue_attributes);
		if (mail_queue <= -1) {
			perror("[Postman] - Couldn\' open the MQ during init\n");
		}

		check = mq_close(mail_queue);
		if (check != 0) {
			perror("[Postman] - Issue while closing the MQ during init\n");
		}
		check = pthread_create(&dispenser_manager_thread, NULL, (void*)&DispenserManager_run, NULL);
		if (check != 0) {
			perror("[Postman] - Issue while creating postman thread\n");
		}
}

static void *DispenserManager_run() {
	DispenserMqMsg msg;
	Dispenser* this;
	DispenserManager_read_backup();
	while(dispenser_list->first_dispenser != NULL) {
		msg = DispenserManager_mq_receive();
		this = DispenserManager_find_dispenser(msg.id);
		Dispenser_run(this, msg);
	}
	return NULL;
}

static void DispenserManager_read_backup() {
	//TODO function that read the backup file
	DispenserManager_add_dispenser(0, "DEFAULT_PRODUCT", 100, 100);
}

void DispenserManager_send_detailed_dispenser(Dispenser* this) {
	Application_detailed_dispenser(this);
}
//TODO
/*static bool DispenserManager_exist(Dispenser_Id id) {
	assert(dispenser_list != NULL);
	bool has_been_found = false;

	Dispenser* current_dispenser = dispenser_list->first_dispenser;
	if(current_dispenser->id != id){
		has_been_found = true;
	}
		while (current_dispenser->next_dispenser != NULL && !has_been_found) {

			if(current_dispenser->next_dispenser->id == id){
				has_been_found = true;
			} else {
				current_dispenser = current_dispenser->next_dispenser;
			}
		}
	return has_been_found;

}*/

void DispenserManager_add_dispenser(Dispenser_Id id, char* product, Battery battery, Filling filling) {
	if(dispenser_list == NULL) {
		printf("initialisation\n");
		dispenser_list = DispenserManager_initialisation();
	}
	assert(dispenser_list != NULL);
	Dispenser* new_dispenser = Dispenser_create(id, product, battery, filling);
	Dispenser* old_first_dispenser = dispenser_list->first_dispenser;
	new_dispenser->next_dispenser = old_first_dispenser;
	dispenser_list->first_dispenser = new_dispenser;
}

Dispenser* DispenserManager_get_list(){
	return dispenser_list->first_dispenser;
}

void DispenserManager_free_dispenser(Dispenser_Id id) {
	assert(dispenser_list != NULL);
	Dispenser* before_deleted_dispenser;
	Dispenser* future_deleted_dispenser;

	if(dispenser_list->first_dispenser->id == id) {
		future_deleted_dispenser = dispenser_list->first_dispenser;
		dispenser_list->first_dispenser = dispenser_list->first_dispenser->next_dispenser;
	} else {
		before_deleted_dispenser = DispenserManager_find_before_dispenser(id);
		future_deleted_dispenser = before_deleted_dispenser->next_dispenser;
		before_deleted_dispenser->next_dispenser = future_deleted_dispenser->next_dispenser;
	}
	Dispenser_destroy(future_deleted_dispenser);
}

static Dispenser* DispenserManager_find_before_dispenser(Dispenser_Id id) {
	assert(dispenser_list != NULL);
	bool has_been_found = false;
	int nbr_de_boucle = 0;

	Dispenser* current_dispenser = dispenser_list->first_dispenser;
	Dispenser* result = NULL;
	if(current_dispenser->id == id){
		has_been_found = true;
	}
	do {
		nbr_de_boucle++;
		if(current_dispenser->next_dispenser->id ==  id) {
			has_been_found = true;
			result = current_dispenser;
		} else {
			current_dispenser = current_dispenser->next_dispenser;
		}
	} while (current_dispenser->next_dispenser != NULL && !has_been_found);

	return result;


}

Dispenser* DispenserManager_find_dispenser(Dispenser_Id id) {
	assert(dispenser_list != NULL);
	bool has_been_found = false;

	Dispenser* current_dispenser = dispenser_list->first_dispenser;
		while (current_dispenser != NULL && !has_been_found) {
			if(current_dispenser->id == id){

				has_been_found = true;
			} else {
				current_dispenser = current_dispenser->next_dispenser;
			}
		}
		if (!has_been_found){
			current_dispenser = (void*) NULL;
		}
		return current_dispenser;
}

void DispenserManager_update_battery(Dispenser_Id id, Battery battery) {
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	Dispenser_set_battery(dispenser, battery);
}

void DispenserManager_update_filling(Dispenser_Id id, Filling filling) {
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	Dispenser_set_filling(dispenser, filling);
}

void DispenserManager_update_product(Dispenser_Id id, char* product_name) {
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	Dispenser_set_product(dispenser, product_name);
}

void DispenserManager_update_last_wash_date(Dispenser_Id id) {
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	Dispenser_set_current_date(dispenser);
}

uint8_t DispenserManager_get_battery(Dispenser_Id id) {
	Battery battery = 0;
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	if(dispenser != NULL){
		battery = Dispenser_get_battery(dispenser);
	}
	return (uint8_t)battery;
}

uint8_t DispenserManager_get_filling(Dispenser_Id id) {
	Filling filling = 0;
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	if(dispenser != NULL) {
		filling = Dispenser_get_filling(dispenser);
	}
	return  (uint8_t)filling;
}

char* DispenserManager_get_product(Dispenser_Id id) {
	char* product_name;
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	if(dispenser != NULL) {
		product_name = Dispenser_get_product_name(dispenser);
	} else {
		product_name = "don\'t exist";
	}
	return product_name;
}

int DispenserManager_get_cleaning_day(Dispenser_Id id ) {
	int day = -1;
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	if(dispenser != NULL) {
		day = Dispenser_get_day(dispenser);
	}
	return day;
}

int DispenserManager_get_cleaning_year(Dispenser_Id id) {
	int year = -1;
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	if(dispenser != NULL) {
		year = Dispenser_get_year(dispenser);
	}
	return year;
}

void DispenserManager_printf(char* arg) {
	Dispenser* current_dispenser = dispenser_list->first_dispenser;
	printf("---------------------------------------------------------------------------------\n");
	if(current_dispenser == NULL) {
		printf("NO DISPENSER CREATED \n");
	} else {
		do {
			Dispenser_printf(current_dispenser, arg);
			current_dispenser = current_dispenser->next_dispenser;
		} while (current_dispenser != NULL);
	}
	printf("--------------------------- fin printf ------------------------------------------\n");
}

void DispenserManager_send_update(Dispenser* this) {
	Application_update_dispenser(this);
}

static void DispenserManager_mq_send(DispenserMqMsg message) {
	int check;
	DispenserMqMsgAdapter msg_adapter;
	mqd_t mail_queue;
	msg_adapter.data = message;
	mail_queue = mq_open(MQ_DISPENSER_MANAGER_NAME, O_WRONLY);
	if(mail_queue <= -1) {
		perror("[Dispenser_Manager] - Couldn\'t open the postman MQ\n");
		strcpy(msg_adapter.buffer, "\0\0");
	} else {
		check = mq_send(mail_queue, msg_adapter.buffer, sizeof(msg_adapter.buffer), 0);
		if (check <= -1) {
			perror("[Dispenser_Manager] - Couldn\'t write in the MQ\n");
		}
	}
	check = mq_close(mail_queue);
}

static DispenserMqMsg DispenserManager_mq_receive() {
	int check;
	mqd_t mail_queue;
	DispenserMqMsgAdapter msg_adapter;
	mail_queue = mq_open(MQ_DISPENSER_MANAGER_NAME, O_RDONLY);
	if(mail_queue == -1) {
		perror("[Dispenser_Manager] - Couldn\'t open the postman MQ\n");
		strcpy(msg_adapter.buffer,"\0\0");
	} else {
		check = mq_receive(mail_queue, msg_adapter.buffer, MQ_DISPENSER_MANAGER_MSG_SIZE, 0);
		if(check == -1) {
			perror("[Dispenser_Manager] - MQ is empty\n");
		}
	}
	check = mq_close(mail_queue);
	return msg_adapter.data;
}

void DispenserManager_check_message(Dispenser* this) {
	DispenserMqMsg msg;
	if(this->message != NULL) {
		msg.event = E_MESSAGE;
	} else {
		msg.event = E_NO_MESSAGE;
	}
	msg.battery = Dispenser_get_battery(this);
	msg.filling = Dispenser_get_filling(this);
	msg.id = Dispenser_get_id(this);
	DispenserManager_mq_send(msg);
}

void DispenserManager_check_conformity_data(DispenserMqMsg message) {
	DispenserMqMsg msg;
	msg.filling = message.filling;
	msg.battery = message.battery;
	msg.id = message.id;
	if(message.battery > 100 || message.filling > 100) {
		msg.event = E_INVALID_DATA;
	} else {
		msg.event = E_VALID_DATA;
	}
	DispenserManager_mq_send(msg);
}

void DispenserManager_compromised_dispenser_event(Dispenser* this) {
	DispenserMqMsg msg;
	msg.battery = Dispenser_get_battery(this);
	msg.filling = Dispenser_get_filling(this);
	msg.id = Dispenser_get_id(this);
	msg.event = E_INF_CPT_INVALID_MESSAGE;
	DispenserManager_mq_send(msg);
}

void DispenserManager_not_compromised_dispenser_event(Dispenser* this) {
	DispenserMqMsg msg;
	msg.battery = Dispenser_get_battery(this);
	msg.filling = Dispenser_get_filling(this);
	msg.id = Dispenser_get_id(this);
	msg.event = E_MAX_CPT_INVALID_MESSAGE;
	DispenserManager_mq_send(msg);
}

void DispenserManager_lost_dispenser_event(Dispenser* this) {
	DispenserMqMsg msg;
	msg.battery =  Dispenser_get_battery(this);
	msg.filling = Dispenser_get_filling(this);
	msg.id = Dispenser_get_id(this);
	msg.event = E_SUP_CPT_TIME_SINCE_LAST_MESSAGE;
	DispenserManager_mq_send(msg);
}

void DispenserManager_not_lost_dispenser_event(Dispenser* this) {
	DispenserMqMsg msg;
	msg.battery =  Dispenser_get_battery(this);
	msg.filling = Dispenser_get_filling(this);
	msg.id = Dispenser_get_id(this);
	msg.event = E_INF_CPT_TIME_SINCE_LAST_MESSAGE;
	DispenserManager_mq_send(msg);
}

void DispenserManager_ask_detailed_dispenser(Dispenser_Id id) {
	Dispenser* this;
	this = DispenserManager_find_dispenser(id);
	DispenserMqMsg msg;
	msg.battery = Dispenser_get_battery(this);
	msg.filling = Dispenser_get_filling(this);
	msg.id = id;
	msg.event = E_ASK_DETAILED;
	DispenserManager_mq_send(msg);
}

void DispenserManager_ask_all_update() {
	Dispenser* current_dispenser = dispenser_list->first_dispenser;
	while (current_dispenser != NULL) {
		DispenserManager_ask_update(current_dispenser);
		current_dispenser = current_dispenser->next_dispenser;
	}
}

static void DispenserManager_ask_update(Dispenser* this) {
	DispenserMqMsg message;
	message.battery = Dispenser_get_battery(this);
	message.filling = Dispenser_get_filling(this);
	message.id = Dispenser_get_id(this);
	message.event = E_ASK_UPDATE;
	DispenserManager_mq_send(message);
}

void DispenserManager_received_message(Dispenser_Id id, Battery battery, Filling filling) {
	DispenserMqMsg message;
	message.id = id;
	message.battery = battery;
	message.filling = filling;
	message.event = E_RECEIVED_MESSAGE;
	DispenserManager_mq_send(message);
}

void DispenserManager_send_message_to_dispenser(Dispenser_Id id, MessageToSend* message) {
	Translator_send_message((uint8_t)id, message->message);
}

void DispenserManager_send_warning_broken_dispenser(Dispenser_Id id) {
	Application_warn_dispenser_is_broken(id);
}
void DispenserManager_prepare_destroy_dispenser(Dispenser_Id id) {
	DispenserMqMsg message;
	message.id = id;
	Dispenser* this = DispenserManager_find_dispenser(id);
	message.battery = this->battery;
	message.filling = this->filling;
	message.event = E_DESTROY;
	DispenserManager_mq_send(message);
}

