/*
 * Watchdog.h
 *
 *  Created on: 17 déc. 2017
 *      Author: sorlie
 */

#ifndef SRC_WATCHDOG_H_
#define SRC_WATCHDOG_H_

#include <time.h>
#include <stdint.h>

typedef struct Watchdog_t Watchdog;

/**
 * @brief This is the function called by the watchdog when de timer is over
 */
typedef void (*WatchdogCallback)(Watchdog *this);

struct Watchdog_t
{
	timer_t timer;
	uint32_t delay; /**< configured delay */
	WatchdogCallback callback_function; /**< function to be called at delay expiration */
};



/* This is the structure of the object Watchdog */

/* Include after the declaration of the structure Watchdog */
//#include "COListener.h"

/**
 * @brief Constructor of the Watchdog object
 *
 * @param[in] delay : The delay in ms to call the callback function
 * @param[in, out] callback : The callback function to call when the delay is over
 *
 */
extern Watchdog *Watchdog_init (uint32_t delay, WatchdogCallback callback);

/**
 * @brief Arms the Watchdog
 *
 * @param[in, out] this : An object Watchdog object arms
 *
 */
extern void Watchdog_start (Watchdog *this);

/**
 * @brief Disarme the Watchdog
 *
 * @param[in, out] this : An object Watchdog object to disarme
 *
 */
extern void Watchdog_cancel (Watchdog *this);


#endif /* SRC_WATCHDOG_H_ */
