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

typedef struct {
	char* message;
}MessageToSend;

extern bool MessageToSend_exist(MessageToSend*);
extern void MessageToSend_destroy(MessageToSend*);

extern MessageToSend* MessageToSend_get_acknowledge_message();

extern MessageToSend* MessageToSend_get_repeat_message();

#endif /* SRC_MESSAGETOSEND_H_ */
