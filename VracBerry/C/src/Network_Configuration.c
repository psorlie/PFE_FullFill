/*
 * Network_Configuration.c
 *
 *  Created on: 15 déc. 2017
 *      Author: sorlie
 */

#include <stdint.h>
#include <pthread.h>
#include <mqueue.h>
#include <stdio.h>

#include "Network_Configuration.h"
#include "Watchdog.h"



static uint32_t wake_up_time_since_midnight_in_minutes;
static uint32_t sleep_time_since_midnight_in_minutes;
static uint16_t time_between_emission_in_minutes;
static uint16_t time_between_cleaning_in_days;

static NetworkState network_state = S_NETWORK_INIT;

static NetworkTransition g_networkStateMachine[NETWORK_NB_STATE][NETWORK_NB_EVENT] = {
		[S_NETWORK_INIT][E_NETWORK_SLEEPING] =
		{S_NETWORK_SLEEPING, A_NETWORK_LAUNCH_SLEEP_WATCHDOG},
		[S_NETWORK_INIT][E_NETWORK_AWAKE] =
		{S_NETWORK_AWAKE, A_NETWORK_LAUNCH_AWAKE_WATCHDOG},

		[S_NETWORK_SLEEPING][E_NETWORK_DEATH] =
		{S_NETWORK_DEATH, A_NETWORK_DEATH},
		[S_NETWORK_SLEEPING][E_NETWORK_SET_NEW_CONFIGURATION] =
		{S_NETWORK_SET_NEW_CONFIGURATION, A_NETWORK_SET_NEW_CONFIGURATION},
		[S_NETWORK_SLEEPING][E_NETWORK_AWAKE] =
		{S_NETWORK_AWAKE, A_NETWORK_CHANGE_MODE_TO_AWAKE},
		[S_NETWORK_SLEEPING][E_NETWORK_ASK_CONFIGURATION] =
		{S_NETWORK_SLEEPING, A_NETWORK_RETURN_CONFIGURATION},

		[S_NETWORK_AWAKE][E_NETWORK_DEATH] =
		{S_NETWORK_DEATH, A_NETWORK_DEATH},
		[S_NETWORK_AWAKE][E_NETWORK_SET_NEW_CONFIGURATION] =
		{S_NETWORK_SET_NEW_CONFIGURATION, A_NETWORK_SET_NEW_CONFIGURATION},
		[S_NETWORK_AWAKE][E_NETWORK_SLEEPING] =
		{S_NETWORK_SLEEPING, A_NETWORK_CHANGE_MODE_TO_SLEEP},
		[S_NETWORK_AWAKE][E_NETWORK_ASK_CONFIGURATION] =
		{S_NETWORK_AWAKE, A_NETWORK_RETURN_CONFIGURATION},

		[S_NETWORK_SET_NEW_CONFIGURATION][E_NETWORK_AWAKE] =
		{S_NETWORK_AWAKE, A_NETWORK_LAUNCH_AWAKE_WATCHDOG},
		[S_NETWORK_SET_NEW_CONFIGURATION][E_NETWORK_SLEEPING] =
		{S_NETWORK_SLEEPING, A_NETWORK_LAUNCH_SLEEP_WATCHDOG}
};

static Watchdog* watchdog;

static pthread_t network_thread;

static void* Network_Configuration_run();

static void Network_Configuration_performAction(NetworkAction, NetworkMqMsg);

static void Network_Configuration_mq_send(NetworkMqMsg);

static NetworkMqMsg Network_Configuration_mq_receive();

static uint32_t Network_Configuration_how_many_minutes_until_wake_up();

static uint32_t Network_Configuration_how_many_minutes_until_sleep();

static void Network_Configuration_launch_sleep_watchdog();

static void Network_Configuration_launch_awake_watchdog();

static void Network_Configuration_prepare_sleeping_mode();

static void Network_Configuration_prepare_awake_mode();

static void Network_Configuration_generate_event_for_watchdog();

static void Network_Configuration_change_configuration (int, int, int, int, int, int);


static void Network_Configuration_mq_send(NetworkMqMsg message) {
	int check;
	NetworkMqMsgAdapter msg_adapter;
	mqd_t mail_queue;
	msg_adapter.data = message;
	mail_queue = mq_open(MQ_DISPENSER_MANAGER_NAME, O_WRONLY);
	if(mail_queue <= -1) {
		perror("[Network Configuration] - Couldn\'t open the network configuration MQ\n");
		strcpy(msg_adapter.buffer, "\0\0");
	} else {
		check = mq_send(mail_queue, msg_adapter.buffer, sizeof(msg_adapter.buffer), 0);
		if (check <= -1) {
			perror("[Network Configuration] - Couldn\'t write in the MQ\n");
		}
	}
	check = mq_close(mail_queue);
}

static NetworkMqMsg Network_Configuration_mq_receive() {
	int check;
	mqd_t mail_queue;
	NetworkMqMsgAdapter msg_adapter;
	mail_queue = mq_open(MQ_NETWORK_CONFIGURATION_NAME, O_RDONLY);
	if(mail_queue == -1) {
		perror("[Network Configuration] - Couldn\'t open the network configuration MQ\n");
		strcpy(msg_adapter.buffer,"\0\0");
	} else {
		check = mq_receive(mail_queue, msg_adapter.buffer, MQ_NETWORK_CONFIGURATION_MSG_SIZE, 0);
		if(check == -1) {
			perror("[Network Configuration] - MQ is empty\n");
		}
	}
	check = mq_close(mail_queue);
	return msg_adapter.data;
}


void Network_Configuration_init() {

	int check = 0;
	wake_up_time_since_midnight_in_minutes = DEFAULT_WAKEUP_HOURS * 60 + DEFAULT_WAKEUP_MINUTES;
	sleep_time_since_midnight_in_minutes = DEFAULT_SLEEPING_HOURS * 60 + DEFAULT_SLEEPING_MINUTES;
	time_between_emission_in_minutes = DEFAULT_DELAY_BETWEEN_MESSAGE;
	time_between_cleaning_in_days = DEFAULT_DAY_BETWEEN_CLEANING;

	check = pthread_create(&network_thread, NULL, (void*)&Network_Configuration_run, NULL);

	if(check == 0){
		perror("[Network-Configuration] - Issue while creating the thread");
	}

	if(Network_Configuration_how_many_minutes_until_wake_up() \
					< Network_Configuration_how_many_minutes_until_sleep()) {
		Network_Configuration_prepare_awake_mode();
	} else {
		Network_Configuration_prepare_sleeping_mode();
	}
}

static void* Network_Configuration_run() {
	NetworkAction action;
	NetworkMqMsg message;
	while(1) {
		message = Network_Configuration_mq_receive();
		if(g_networkStateMachine[network_state][message.event].futurState == S_NETWORK_NOP){
			perror("[Network Configuration] - Perte d'évènement\n");
		} else {
			action = g_networkStateMachine[network_state][message.event].futurAction;
			network_state = g_networkStateMachine[network_state][message.event].futurState;
			Network_Configuration_performAction(action, message);
		}
	}
	return NULL;
}

static void Network_Configuration_performAction(NetworkAction action, NetworkMqMsg message) {
	switch(action) {
	case A_NETWORK_LAUNCH_SLEEP_WATCHDOG:
		Network_Configuration_launch_awake_watchdog();
		break;
	case A_NETWORK_LAUNCH_AWAKE_WATCHDOG:
		Network_Configuration_launch_sleep_watchdog();
		break;
	case A_NETWORK_CHANGE_MODE_TO_SLEEP:
		DispenserManager_end_of_the_day();
		Watchdog_cancel(watchdog);
		Network_Configuration_launch_awake_watchdog();
		break;
	case A_NETWORK_CHANGE_MODE_TO_AWAKE:
		DispenserManager_morning();
		Watchdog_cancel(watchdog);
		Network_Configuration_launch_sleep_watchdog();
		break;
	case A_NETWORK_SET_NEW_CONFIGURATION:
		Watchdog_cancel(watchdog);
		Network_Configuration_change_configuration (message.data_transmitted.configuration.day_cleaning,\
				message.data_transmitted.configuration.minutes_emission,\
				message.data_transmitted.configuration.hour_wake_up,\
				message.data_transmitted.configuration.minutes_wake_up,\
				message.data_transmitted.configuration.hour_sleep,\
				message.data_transmitted.configuration.minutes_sleep);
		Network_Configuration_generate_event_for_watchdog();
		break;
	case A_NETWORK_RETURN_CONFIGURATION:
		Application_send_current_configuration(wake_up_time_since_midnight_in_minutes,\
				sleep_time_since_midnight_in_minutes, time_between_emission_in_minutes,\
				time_between_cleaning_in_days);
		break;
	case A_NETWORK_DEATH:
		pthread_cancel(network_thread);
		break;
	default:
		perror("[Network Configuration] - Unstable state machine\n");
	}
}



static uint32_t Network_Configuration_how_many_minutes_until_wake_up() {
	time_t rawtime;
	struct tm * timeinfo;
	uint32_t time_in_minutes_until_wake_up, total_minutes;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	total_minutes = ((timeinfo->tm_hour) * 60) + timeinfo->tm_min;
	if(wake_up_time_since_midnight_in_minutes > total_minutes) {
		time_in_minutes_until_wake_up = wake_up_time_since_midnight_in_minutes - total_minutes;
	} else {
		time_in_minutes_until_wake_up = 24 * 60; // minutes in a day
		time_in_minutes_until_wake_up -= (total_minutes - wake_up_time_since_midnight_in_minutes);
	}
	return time_in_minutes_until_wake_up;
}

static uint32_t Network_Configuration_how_many_minutes_until_sleep() {
	time_t rawtime;
	struct tm * timeinfo;
	uint32_t time_in_minutes_until_sleep, total_minutes;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	total_minutes = ((timeinfo->tm_hour) * 60) + timeinfo->tm_min;
	if(sleep_time_since_midnight_in_minutes > total_minutes) {
		time_in_minutes_until_sleep = sleep_time_since_midnight_in_minutes - total_minutes;
	} else {
		time_in_minutes_until_sleep = 24 * 60; // minutes in a day
		time_in_minutes_until_sleep -= (total_minutes - sleep_time_since_midnight_in_minutes);
	}
	return time_in_minutes_until_sleep;
}

static void Network_Configuration_prepare_sleeping_mode() {
	NetworkMqMsg message;
	message.event = E_NETWORK_SLEEPING;
	Network_Configuration_mq_send(message);
}

static void Network_Configuration_prepare_awake_mode() {
	NetworkMqMsg message;
	message.event = E_NETWORK_AWAKE;
	Network_Configuration_mq_send(message);
}

static void Network_Configuration_launch_sleep_watchdog() {
	watchdog = Watchdog_init(Network_Configuration_how_many_minutes_until_sleep(),\
			Network_Configuration_end_of_the_day);
	Watchdog_start(watchdog);
}

static void Network_Configuration_launch_awake_watchdog() {
	watchdog = Watchdog_init(Network_Configuration_how_many_minutes_until_wake_up(),\
			Network_Configuration_morning);
	Watchdog_start(watchdog);
}

void Network_Configuration_morning() {
	NetworkMqMsg message;
	message.event = E_NETWORK_AWAKE;
	Network_Configuration_mq_send(message);
}

void Network_Configuration_end_of_the_day() {
	NetworkMqMsg message;
	message.event = E_NETWORK_SLEEPING;
	Network_Configuration_mq_send(message);
}

static void Network_Configuration_generate_event_for_watchdog() {
	NetworkMqMsg message;
	if(Network_Configuration_how_many_minutes_until_wake_up() >\
			Network_Configuration_how_many_minutes_until_sleep()) {
		message.event = E_NETWORK_AWAKE;
	} else {
		message.event = E_NETWORK_SLEEPING;
	}
	Network_Configuration_mq_send(message);
}

void Network_Configuration_set_new_configuration(int day_cleaning, int minutes_emission, \
		int hour_wake_up, int minutes_wake_up, int hour_sleep, int minutes_sleep) {
	bool is_valid = true;
	NetworkMqMsg message;
	//TODO
	perror("à voir ce qu'on reçoit, ici on check la validité des données");

	if(is_valid) {
		message.event = E_NETWORK_SET_NEW_CONFIGURATION;
		message.data_transmitted.configuration.day_cleaning = day_cleaning;
		message.data_transmitted.configuration.minutes_emission = minutes_emission;
		message.data_transmitted.configuration.hour_wake_up = hour_wake_up;
		message.data_transmitted.configuration.minutes_wake_up = minutes_wake_up;
		message.data_transmitted.configuration.hour_sleep = hour_sleep;
		message.data_transmitted.configuration.minutes_sleep = minutes_sleep;
		Application_warn_configuration_accepted();
		Network_Configuration_mq_send(message);
	} else {
		Application_warn_configuration_refused();
	}
}

static void Network_Configuration_change_configuration (int day_cleaning, int minutes_emission, \
	int hour_wake_up, int minutes_wake_up, int hour_sleep, int minutes_sleep) {
	wake_up_time_since_midnight_in_minutes = (uint32_t)(hour_wake_up * 60 + minutes_wake_up);
	sleep_time_since_midnight_in_minutes = (uint32_t)(hour_sleep * 60 + minutes_sleep);
	time_between_emission_in_minutes = (uint16_t)minutes_emission;
	time_between_cleaning_in_days = (uint16_t)day_cleaning;
}

