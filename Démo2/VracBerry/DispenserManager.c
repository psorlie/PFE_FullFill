/*
 * DispenserManager.c
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <pthread.h>
#include <inttypes.h>

#include "Proxy.h"
#include "DispenserManager.h"
#include "Postman.h"
#include "Postman_UART.h"
//#include "Translator.h"


struct Dispenser_list_t {
	Dispenser* first_dispenser;
};

static pthread_t dispenser_manager_thread;

static Dispenser_list* dispenser_list;


/**************************************** Static fonctions ***************************************/

static void* DispenserManager_run();

Dispenser* DispenserManager_find_dispenser(Dispenser_Id);

static void DispenserManager_ask_update(Dispenser*);

static void DispenserManager_add_dispenser_from_backup(Dispenser_Id, char*, Battery, Filling, int, int, int, int, int);


/*
static Dispenser* DispenserManager_find_before_dispenser(Dispenser_Id);

static Dispenser_list* DispenserManager_initialisation();

static void DispenserManager_mq_send(DispenserMqMsg);

static DispenserMqMsg DispenserManager_mq_receive();

static void DispenserManager_read_backup();
*/


/*************************************** Get ***********************************************/

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

int DispenserManager_get_cleaning_day_of_month(Dispenser_Id id ) {
	int day = -1;
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	if(dispenser != NULL) {
		day = Dispenser_get_day_of_month(dispenser);
	}
	return day;
}

int DispenserManager_get_cleaning_day_of_year(Dispenser_Id id ) {
	int day = -1;
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	if(dispenser != NULL) {
		day = Dispenser_get_day_of_year(dispenser);
	}
	return day;
}

int DispenserManager_get_cleaning_month(Dispenser_Id id ) {
	int day = -1;
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	if(dispenser != NULL) {
		day = Dispenser_get_month(dispenser);
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


/*************************************** Affichage ********************************************/


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


/********************************** Liste chaînée de dispenser *******************************************/

static Dispenser_list* DispenserManager_initialisation() {
	assert(dispenser_list == NULL);

	Dispenser_list* list = malloc(sizeof(Dispenser_list));
	if(list == NULL){
		exit(0);
	}
	return list;
}


Dispenser* DispenserManager_get_list(){
	return dispenser_list->first_dispenser;
}


Dispenser* DispenserManager_add_dispenser(Dispenser_Id id, char* product, Battery battery, Filling filling) {
	if(dispenser_list == NULL) {
		printf("initialisation\n");
		dispenser_list = DispenserManager_initialisation();
	}
	assert(dispenser_list != NULL);
	Dispenser* new_dispenser = Dispenser_create(id, product, battery, filling);
	Dispenser* old_first_dispenser = dispenser_list->first_dispenser;
	new_dispenser->next_dispenser = old_first_dispenser;
	dispenser_list->first_dispenser = new_dispenser;
	return new_dispenser;
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


/******************************************* Initialisation ******************************************/


void DispenserManager_init() {

	struct mq_attr mail_queue_attributes;
	int check;
	mqd_t mail_queue;
	mail_queue_attributes.mq_maxmsg = MQ_DISPENSER_MANAGER_MSG_COUNT;
	mail_queue_attributes.mq_msgsize = MQ_DISPENSER_MANAGER_MSG_SIZE;
	mail_queue_attributes.mq_flags = 0;

	check = mq_unlink(MQ_DISPENSER_MANAGER_NAME);
	if(check != 0) {
		perror("[DispenserManager] - MQ already exist ?\n");
	}

	mail_queue = mq_open(MQ_DISPENSER_MANAGER_NAME, O_CREAT, 0777, &mail_queue_attributes);
	if (mail_queue <= -1) {
		perror("[DispenserManager] - Couldn\' open the MQ during init\n");
	}

	check = mq_close(mail_queue);
	if (check != 0) {
		perror("[DispenserManager] - Issue while closing the MQ during init\n");
	}

	check = pthread_create(&dispenser_manager_thread, NULL, (void*)&DispenserManager_run, NULL);
	if (check != 0) {
		perror("[DispenserManager] - Issue while creating DispenserManager thread\n");
	}
	
}


static void DispenserManager_read_backup() {
	int state= 0;
	uint8_t id, battery, filling = 0;
	int day_of_month, day_of_year, month, year = 0;
	char product [PRODUCT_SIZE] = "";
	FILE* file_identifier;
	fpos_t pos;
	int pos_init = 0;
	bool is_not_over = true;
	if((file_identifier = fopen("backup.txt", "rw+")) == NULL) {
		DispenserManager_add_dispenser(0, DEFAULT_PRODUCT_NAME, 100, 100);		// TODO : pourquoi??
	} else {
		if(pos_init) {
			fsetpos(file_identifier, &pos);
		}
		while(is_not_over) {
			//printf("ggg\n");
			if (fscanf(file_identifier, "id=%" PRIu8 " battery=%" PRIu8 " filling=%" PRIu8 " product=%s day_of_month=%d day_of_year=%d month=%d year=%d state=%d", 
										&id, &battery, &filling, product, &day_of_month, &day_of_year, &month, &year, &state)) {
				DispenserManager_add_dispenser_from_backup(id, product, battery, filling, day_of_month, day_of_year, month, year, state);
				fgetpos(file_identifier, &pos);
				pos_init = pos_init + 1;
				//printf("id=%" PRIu8 "battery=%" PRIu8 " filling=%" PRIu8 " product=%s day_of_month=%d day_of_year=%d month=%d year=%d state=%d", 
					//id, battery, filling, product, day_of_month, day_of_year, month, year, state);
			} else {
				is_not_over = false;
			}
		}
	}
	fclose(file_identifier);
}

static void DispenserManager_add_dispenser_from_backup(Dispenser_Id id, char* product, Battery battery, Filling filling,
														int day_of_month, int day_of_year, int month, int year, int state) {
	Dispenser* this = DispenserManager_add_dispenser(id, product, battery, filling);
	Dispenser_set_specified_date(this, day_of_month, day_of_year, month, year);
	//Proxy_send_new(this);
	this->state = (DispenserState)state;
}


/**************************************** BAL *****************************************/


static void DispenserManager_mq_send(DispenserMqMsg message) {
	//printf("JE SUIS ICI");
	int check;
	DispenserMqMsgAdapter msg_adapter;
	mqd_t mail_queue;
	msg_adapter.data = message;
	mail_queue = mq_open(MQ_DISPENSER_MANAGER_NAME, O_WRONLY);
	if(mail_queue <= -1) {
		perror("[Dispenser_Manager] - Couldn\'t open the Dispenser_Manager MQ\n");
		strcpy(msg_adapter.buffer, "\0\0");
	} else {
		check = mq_send(mail_queue, msg_adapter.buffer, sizeof(msg_adapter.buffer), 0);
		if (check <= -1) {
			perror("[Dispenser_Manager] - Couldn\'t write in the MQ\n");
		}
	}
	check = mq_close(mail_queue);
	//printf("GLA-GLA OH LALAAAAA");
}

static DispenserMqMsg DispenserManager_mq_receive() {
	//printf("JE SUIS LA");
	int check;
	mqd_t mail_queue;
	DispenserMqMsgAdapter msg_adapter;
	mail_queue = mq_open(MQ_DISPENSER_MANAGER_NAME, O_RDONLY);
	if(mail_queue == -1) {
		perror("[Dispenser_Manager] - Couldn\'t open the DispenserManager MQ\n");
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


/***************************************** MAE ****************************************/


static void *DispenserManager_run() {
	printf("je suis run\n");
	DispenserMqMsg msg;
	Dispenser* this;
	printf("avt backup\n");
	//DispenserManager_read_backup();
	DispenserManager_add_dispenser(1, "Cacahuètes", 52, 65);
	printf("après backup\n");
	DispenserManager_printf("");
	while(dispenser_list->first_dispenser != NULL) {
		msg = DispenserManager_mq_receive();
		this = DispenserManager_find_dispenser(msg.id);
		printf("id %d\n", msg.id);
		Dispenser_printf(this, "");
		if(this != NULL) {
			Dispenser_run(this, msg);
		} else {
			this = DispenserManager_add_new_detected_dispenser(msg.id, 
					msg.data_transmitted.battery_and_filling.battery, msg.data_transmitted.battery_and_filling.filling);
			//Proxy_send_new(this);
		}
	}
	//printf("run end\n");
	return NULL;
}


void DispenserManager_compromised_dispenser_event(Dispenser* this) {
	DispenserMqMsg msg;
	msg.data_transmitted.battery_and_filling.battery = Dispenser_get_battery(this);
	msg.data_transmitted.battery_and_filling.filling = Dispenser_get_filling(this);
	msg.id = Dispenser_get_id(this);
	msg.event = E_MAX_CPT_INVALID_MESSAGE;
	DispenserManager_mq_send(msg);
}


void DispenserManager_not_compromised_dispenser_event(Dispenser* this) {
	DispenserMqMsg msg;
	msg.data_transmitted.battery_and_filling.battery = Dispenser_get_battery(this);
	msg.data_transmitted.battery_and_filling.filling = Dispenser_get_filling(this);
	msg.id = Dispenser_get_id(this);
	msg.event = E_INF_CPT_INVALID_MESSAGE;
	DispenserManager_mq_send(msg);
}


void DispenserManager_lost_dispenser_event(Dispenser* this) {
	DispenserMqMsg msg;
	msg.data_transmitted.battery_and_filling.battery =  Dispenser_get_battery(this);
	msg.data_transmitted.battery_and_filling.filling = Dispenser_get_filling(this);
	msg.id = Dispenser_get_id(this);
	msg.event = E_HAS_NOT_EMITTED;
	DispenserManager_mq_send(msg);
}


void DispenserManager_not_lost_dispenser_event(Dispenser* this) {
	DispenserMqMsg msg;
	msg.data_transmitted.battery_and_filling.battery =  Dispenser_get_battery(this);
	msg.data_transmitted.battery_and_filling.filling = Dispenser_get_filling(this);
	msg.id = Dispenser_get_id(this);
	msg.event = E_HAS_EMITTED;
	DispenserManager_mq_send(msg);
}


void DispenserManager_prepare_destroy_dispenser(Dispenser_Id id) {
	DispenserMqMsg message;
	message.id = id;
	Dispenser* this = DispenserManager_find_dispenser(id);
	message.data_transmitted.battery_and_filling.battery = this->battery;
	message.data_transmitted.battery_and_filling.filling = this->filling;
	message.event = E_DESTROY;
	DispenserManager_mq_send(message);
}


/**************************************** Network config ************************************/


void DispenserManager_save_in_backup(Dispenser* this) {
	//TODO
	perror("[DispenserManager] - No backup saving function\n");
}


void DispenserManager_check_dispenser_is_dirty(int total_day_diff) {
	//TODO
	perror("[DispenserManager] - Still not calculating if a dispenser is dirty\n");
}


 void DispenserManager_end_of_the_day() {
 	/*
	 Dispenser* this = dispenser_list->first_dispenser;
	 while(this != NULL) {
		 DispenserMqMsg message;
		 message.id = this->id;
		 message.event = E_END_OF_THE_DAY;
		 DispenserManager_mq_send(message);
		 this = this->next_dispenser;
	 }
	 */
 }


 void DispenserManager_morning() {
 	/*
	 Dispenser* this = dispenser_list->first_dispenser;
	 while(this != NULL) {
		 DispenserMqMsg message;
		 message.id = this->id;
		 message.event = E_MORNING;
		 DispenserManager_mq_send(message);
		 this = this->next_dispenser;
	 }
	 */
 }


void DispenserManager_change_configuration(uint16_t time_between_emission) {
	/*
	 Dispenser* this = dispenser_list->first_dispenser;
	 while(this != NULL) {
			 Dispenser_set_message(this, time_between_emission);
			 this = this->next_dispenser;
		 }
	*/
}


/***************************************** Appli ****************************************/


void DispenserManager_send_detailed_dispenser(Dispenser* this) {
	Proxy_send_detail(this);
}


void DispenserManager_send_update(Dispenser* this) {
	Proxy_send_update(this);
}


void DispenserManager_send_warning_broken_dispenser(Dispenser_Id id) {
	//Application_warn_dispenser_is_broken(id);
	printf("broken dispenser");
}


void DispenserManager_send_warning_lost_dispenser(Dispenser_Id id) {
	//Application_warn_dispenser_is_lost(id);
	printf("lost dispenser");
}


void DispenserManager_ask_product_name(Dispenser* this) {
	/*
	Proxy_send_new(this);
	*/
}


void DispenserManager_prepare_set_current_wash_date(Dispenser* dispenser) {
	Dispenser_set_current_date(dispenser);
    Proxy_send_detail(dispenser);
}


void DispenserManager_prepare_set_new_product_name(Dispenser_Id id, char* name) {
	 DispenserMqMsg message;
	 message.id = id;
	 message.event = E_SET_NEW_PRODUCT_NAME;
	 int size = strlen(name)+1;
	 snprintf(message.data_transmitted.product, size, "%s", name);
	 DispenserManager_mq_send(message);
}


void DispenserManager_prepare_set_current_date(Dispenser_Id id) {
	 DispenserMqMsg message;
	 message.id = id;
	 message.event = E_SET_CURRENT_DATE;
	 DispenserManager_mq_send(message);
}


void DispenserManager_ask_detailed_dispenser(Dispenser_Id id) {
	Dispenser* this;
	this = DispenserManager_find_dispenser(id);
	DispenserMqMsg msg;
	msg.data_transmitted.battery_and_filling.battery = Dispenser_get_battery(this);
	msg.data_transmitted.battery_and_filling.filling = Dispenser_get_filling(this);
	msg.id = id;
	msg.event = E_ASK_DETAILED;
	DispenserManager_mq_send(msg);
}


void DispenserManager_ask_all_update() {
	printf("je suis là");
	Dispenser* current_dispenser = dispenser_list->first_dispenser;
	while (current_dispenser != NULL) {
		printf("bouh!");
		DispenserManager_ask_update(current_dispenser);
		current_dispenser = current_dispenser->next_dispenser;
	}
}


static void DispenserManager_ask_update(Dispenser* this) {
	DispenserMqMsg message;
	message.data_transmitted.battery_and_filling.battery = Dispenser_get_battery(this);
	message.data_transmitted.battery_and_filling.filling = Dispenser_get_filling(this);
	message.id = Dispenser_get_id(this);
	message.event = E_ASK_UPDATE;
	DispenserManager_mq_send(message);
}



/**************************************** Zigbee *****************************************/


Dispenser* DispenserManager_add_new_detected_dispenser(Dispenser_Id id, Battery battery, Filling filling) {
	Dispenser* this;
	this = DispenserManager_add_dispenser(id, DEFAULT_PRODUCT_NAME, battery, filling);
	return this;
}


void DispenserManager_check_message(Dispenser* this) {
	DispenserMqMsg msg;
	if(this->message != NULL) {
		msg.event = E_MESSAGE;
	} else {
		msg.event = E_NO_MESSAGE;
	}
	msg.data_transmitted.battery_and_filling.battery = Dispenser_get_battery(this);
	msg.data_transmitted.battery_and_filling.filling = Dispenser_get_filling(this);
	msg.id = Dispenser_get_id(this);
	DispenserManager_mq_send(msg);
}


void DispenserManager_check_conformity_data(DispenserMqMsg message) {
	DispenserMqMsg msg;
	msg.data_transmitted.battery_and_filling.filling = message.data_transmitted.battery_and_filling.filling;
	msg.data_transmitted.battery_and_filling.battery = message.data_transmitted.battery_and_filling.battery;
	msg.id = message.id;
	if(message.data_transmitted.battery_and_filling.battery > 100 || message.data_transmitted.battery_and_filling.filling > 100) {
		msg.event = E_INVALID_DATA;
	} else {
		msg.event = E_VALID_DATA;
	}
	DispenserManager_mq_send(msg);
}


void DispenserManager_received_message(Dispenser_Id id, Battery battery, Filling filling) {
	DispenserMqMsg message;
	message.id = id;
	message.data_transmitted.battery_and_filling.battery = battery;
	message.data_transmitted.battery_and_filling.filling = filling;
	message.event = E_RECEIVED_MESSAGE;
	DispenserManager_mq_send(message);
}


void DispenserManager_send_message_to_dispenser(Dispenser_Id id, MessageToSend* message) {
	//Translator_send_message((uint8_t)id, message->message);
}


 void DispenserManager_tell_dispenser_broken(Dispenser_Id id) {
	 //Translator_send_message((uint8_t)id, PRODUCT_BROKEN);
 }


 void DispenserManager_tell_dispenser_repeat(Dispenser_Id id) {
	 //Translator_send_message((uint8_t)id, PRODUCT_REPEAT);
 }





/* TEST */
/*
//Appelé par le dispatcher
void DispenserManager_update_battery(Dispenser_Id id, Battery battery) {
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	Dispenser_set_battery(dispenser, battery);
}

//Appelé par le dispatcher
void DispenserManager_update_filling(Dispenser_Id id, Filling filling) {
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	Dispenser_set_filling(dispenser, filling);
}

//Appelé par le dispatcher
void DispenserManager_update_product(Dispenser_Id id, char* product_name) {
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	Dispenser_set_product(dispenser, product_name);
}

//Appelé par le dispatcher
void DispenserManager_update_last_wash_date(Dispenser_Id id) {
	Dispenser* dispenser = DispenserManager_find_dispenser(id);
	Dispenser_set_current_date(dispenser);
}
*/
