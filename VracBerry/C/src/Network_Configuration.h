/*
 * Network_Configuration.h
 *
 *  Created on: 15 déc. 2017
 *      Author: sorlie
 */

#ifndef SRC_NETWORK_CONFIGURATION_H_
#define SRC_NETWORK_CONFIGURATION_H_

#include <stdbool.h>
#include <stdint.h>

#include "Application.h"
#include "DispenserManager.h"

#define MQ_NETWORK_CONFIGURATION_MSG_SIZE (2048)
#define MQ_NETWORK_CONFIGURATION_NAME "/mq_network_configuration"


#define MINIMAL_INTER_EMISSION_TIME (30)
#define MINIMAL_WAKE_UP_HOURS (6)
#define MINIMAL_SLEEP_HOURS (12)
#define DEFAULT_DAY_BETWEEN_CLEANING (15)
#define DEFAULT_DELAY_BETWEEN_MESSAGE (45)
#define DEFAULT_WAKEUP_HOURS (8)
#define DEFAULT_WAKEUP_MINUTES (30)
#define DEFAULT_SLEEPING_HOURS (22)
#define DEFAULT_SLEEPING_MINUTES (30)

typedef enum {
	S_NETWORK_NOP = 0,
	S_NETWORK_INIT,
	S_NETWORK_SLEEPING,
	S_NETWORK_AWAKE,
	S_NETWORK_SET_NEW_CONFIGURATION,
	S_NETWORK_DEATH,
	NETWORK_NB_STATE
}NetworkState;

typedef enum {
	E_NETWORK_SLEEPING = 0,
	E_NETWORK_AWAKE,
	E_NETWORK_ASK_CONFIGURATION,
	E_NETWORK_SET_NEW_CONFIGURATION,
	E_NETWORK_DEATH,
	NETWORK_NB_EVENT,
}NetworkEvent;

typedef enum {
	A_NETWORK_LAUNCH_SLEEP_WATCHDOG = 0,
	A_NETWORK_LAUNCH_AWAKE_WATCHDOG,
	A_NETWORK_CHANGE_MODE_TO_SLEEP,
	A_NETWORK_CHANGE_MODE_TO_AWAKE,
	A_NETWORK_RETURN_CONFIGURATION,
	A_NETWORK_SET_NEW_CONFIGURATION,
	A_NETWORK_DEATH,
	A_NETWORK_NB_ACTION
}NetworkAction;

typedef struct NetworkTransition_t {
	NetworkState futurState;
	NetworkAction futurAction;
} NetworkTransition;

typedef struct {
	int day_cleaning;
	int minutes_emission;
	int hour_wake_up;
	int minutes_wake_up;
	int hour_sleep;
	int minutes_sleep;
}Configuration;

typedef union {
	Configuration configuration;
	uint32_t time_for_watchdog;
}Network_data_transmitted;

typedef struct {
	NetworkEvent event;
	Network_data_transmitted data_transmitted;
} NetworkMqMsg;

typedef union {
	char buffer[MQ_NETWORK_CONFIGURATION_MSG_SIZE];
	NetworkMqMsg data;
}NetworkMqMsgAdapter;


extern void Network_Configuration_morning();

extern void Network_Configuration_end_of_the_day();

#endif /* SRC_NETWORK_CONFIGURATION_H_ */
