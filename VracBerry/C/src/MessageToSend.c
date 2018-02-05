/*
 * MessageToSend.c
 *
 *  Created on: 16 nov. 2017
 *      Author: sorlie
 */
#include <stdbool.h>
#include <stdlib.h>

#include "MessageToSend.h"

/**
 * @def the default ack message
 */
static const char* BASIC_ACK = "ack";

/**
 * @def the default repeat message
 */
static const char* BASIC_REPEAT = "repeat";

/**
 * @brief This function is called to free the MessageToSend after sending it.
 *
 * @param[in] MessageToSend*, pointer to the message to free.
 */
static void MessageToSend_free(MessageToSend*);

bool MessageToSend_exist(MessageToSend* this) {
	return (this != NULL);
}

void MessageToSend_destroy(MessageToSend* this) {
	if (this != NULL) {
		MessageToSend_free(this);
	}
}

MessageToSend* MessageToSend_get_acknowledge_message() {
	MessageToSend* message = (MessageToSend*)malloc(sizeof(MessageToSend));
	message->message = BASIC_ACK;
	return message;
}

static void MessageToSend_free(MessageToSend* this) {
	free(this);
}

MessageToSend* MessageToSend_get_repeat_message() {
	MessageToSend* message = (MessageToSend*)malloc(sizeof(MessageToSend));
	message->message = BASIC_REPEAT;
	return message;
}


