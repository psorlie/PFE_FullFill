/*
 * MessageToSend.h
 *
 *  Created on: 14 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_MESSAGETOSEND_H_
#define SRC_MESSAGETOSEND_H_

#include <string.h>

#define MESSAGE_SIZE (20)
/**
 * @def MessageToSend, encapsulation of a char* : for the instant just a char, but could be a struct containing
 * all the network parameters ?
 */
typedef struct {
	char* message;
}MessageToSend;

/**
 * @brief This function is called when we want to know if we have a message to send to a dispenser
 *
 * @param[in] MessageToSend*, pointer to the hypothetical message to send
 *
 * @return bool, yes if we have a message, else no
 */
extern bool MessageToSend_exist(MessageToSend*);

/**
 * @brief This function is called when we want to destroy a message in a dispenser after sending it.
 *
 * @param[in] MessageToSend*, pointer to the to-be destroy message
 */
extern void MessageToSend_destroy(MessageToSend*);

/**
 * @brief This function is called when we want to get the basic acknowledge answer to a dispenser
 *
 * @return MessageToSend*, pointer to the default answer to acknowledge what the dispenser sent.
 */
extern MessageToSend* MessageToSend_get_acknowledge_message();

/**
 * @brief This function is called when we want a dispenser to repeat its measure since the data received was
 * corrupted.
 *
 * @return MessageToSend*, pointer to the default answer to ask for a repeat to a dispenser.
 */
extern MessageToSend* MessageToSend_get_repeat_message();

#endif /* SRC_MESSAGETOSEND_H_ */
