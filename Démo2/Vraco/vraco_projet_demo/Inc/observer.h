/*
 * observer.h
 *
 *  Created on: 21 déc. 2017
 *      Author: pierr
 */

#ifndef OBSERVER_H_M0_
#define OBSERVER_H_M0_

#define DEFAULT_LISTENING_DELAY (1)

#define DEFAULT_ID (1)
#define DEFAULT_SAMPLE (0)
#define DEFAULT_BAT_LVL (0)

#define DEFAULT_DELAY_REPEAT_MSG (1000)

#define  DEFAULT_ACK (0)

#define NB_MSG_MAX (1)

#define NB_SENSOR (2)

#define MAX_FAILED (0)

#define DEFAULT_PARAM (0)
#define DEFAULT_CODE (0)
#define DEFAULT_NEW_PARAM (0)


#include <stdint.h>
#include <stdio.h>

typedef enum {
	NOPE_E=0,
	RTC_PERIODIC_ALARM_E,
	CURFEW_END_E,
	CURFEW_BEGINNING_E,
	BOOL_MSG_TODAY_YES_E,
	BOOL_MSG_TODAY_NO_E,
	MEASURE_OVER_E,
	IS_WORTH_E,
	IS_NOT_WORTH_E,
	LISTEN_E,
	WATCHDOG_LAUNCHED_E,
	ACK_DIFF_CURRENT_ID_E,
	NB_MSG_SUP_MAX_E,
	NB_MSG_INF_MAX_E,
	ACK_EQUAL_CURRENT_ID_E,
	TEXT_UPDATE_E,
	TEXT_EMPTY_E,
	BROKEN_E,
	TEXT_REPEAT_E,
	NB_EVENT
} ObserverEvent;

typedef enum{
	NOPE_A=0,
	EVALUATE_MEAS_A,
	IS_WORTH_A,
	LISTEN_A,
	REPEAT_NOT_MY_ID_A,
	CHECK_NB_MSG_ALREADY_SENT_A,
	SEND_AND_WAIT_A,
	ANALYSE_TEXT_A,
	BROKEN_A,
	TEXT_UPDATE_A,
	TEXT_EMPTY_A,
	CURFEW_BEGINNING_A,
	CURFEW_END_A,
	SEND_OLD_VAL_A,
	RESTART_RTC_PERIODIC_ALARM_A,
	RTC_PERIODIC_ALARM_A,
	NB_ACTION
} ObserverAction;

typedef enum{
	NAP_S=0,
	MEASURING_S,
	CHECK_WORTH_S,
	LISTEN_ACK_S,
	ANALYSE_ACK_S,
	CHECK_NB_REPEAT_S,
	CHECK_NEW_PARAM_S,
	CHECK_MSG_BEFORE_SLEEP_S,
	SLEEP_S,
	DEATH_S,
	NB_STATE
} ObserverState;


typedef struct{
	ObserverState futurState;
	ObserverAction futurAction;
} ObserverTransition;

typedef union {
	uint8_t sample;
	char* order;
}Param;

enum commandeCode {
	EMPTY_CODE =0,
	UPDATE_CODE,
	BROKEN_CODE,
	REPEAT_CODE
};
extern void observer_wake_up();
void observer_has_received_nothing();
extern void observer_init();
void SystemClock_Config(void);
void Error_Handler(void);
extern void observer_has_received(char*);

/**Ce n'est pas le bon ID reçun on attends et on répète l'envoi**/
extern void wait_and_free();

/**On analyse l'ack du message, on le parse et on détecte s'il y a un nouveau param**/
extern void analyse_param();



#endif /* OBSERVER_H_M0_ */
