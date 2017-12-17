/*
 * Watchdog.c
 *
 *  Created on: 17 déc. 2017
 *      Author: sorlie
 */


#include "Watchdog.h"
#include <malloc.h>

#include <signal.h>
#include <time.h>
#include <assert.h>

static void Watchdog_free (Watchdog*);

static void main_handler (union sigval);

/**
 * Calls the watchdog callback when the delay of the timer expires
 *
 * @param handlerParam must be the watchdog reference
 */
static void main_handler (union sigval handler_param) {
	Watchdog *watchdog = (Watchdog*) handler_param.sival_ptr;
	watchdog->callback_function(watchdog);
}


Watchdog *Watchdog_init (uint32_t delay, WatchdogCallback callback) {
	Watchdog *result;
	struct sigevent sig_event;
	sig_event.sigev_notify = 0;
	sig_event.sigev_signo = 0;
	sig_event.sigev_value.sival_ptr = NULL;
	sig_event._sigev_un._tid = 0;

	int error_code;


	// allocates and initializes the watchdog's attributes
	result = (Watchdog *)malloc (sizeof(Watchdog));
	assert(result != NULL);


	result->delay = delay;
	result->callback_function = callback;

	// allocates and initializes the sigEvent's attributes
	sig_event.sigev_value.sival_ptr = result;
	sig_event.sigev_notify = SIGEV_THREAD;
	sig_event.sigev_notify_function = main_handler;

	//creation du timer
	error_code = timer_create(CLOCK_REALTIME, &sig_event, &(result->timer));
    assert(error_code == 0);

	return result;
}

void Watchdog_start (Watchdog* this) {
	int error_code;
	struct itimerspec value;
	value.it_interval.tv_nsec = 0;
	value.it_interval.tv_sec = 0;
	value.it_value.tv_nsec = 0;
	value.it_value.tv_sec = this->delay;

	//démarrage du timer
	error_code = timer_settime(this->timer, 0, &value, NULL);
	//errorLog("Function WatchdogStart : Error while setting time", error_code == -1);
    assert(error_code != -1);
}


static void Watchdog_free (Watchdog* this) {
	int error_code;
	struct itimerspec value;

	value.it_value.tv_sec = 0;
	value.it_value.tv_nsec = 0;

	value.it_interval.tv_sec = 0;
	value.it_interval.tv_nsec = 0;

	//arret du timer
	error_code = timer_settime(this->timer, 0, &value, NULL);
    assert(error_code != -1);

    error_code = timer_delete (this->timer);
    assert(error_code == 0);

}

void Watchdog_cancel (Watchdog* this) {
	if(this != NULL){
		Watchdog_free(this);
		//destruction du timer
		free(this);
	}

}
