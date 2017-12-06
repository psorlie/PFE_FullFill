/*
 * MessageToSend.c
 *
 *  Created on: 16 nov. 2017
 *      Author: sorlie
 */
#include <stdbool.h>
#include <stdlib.h>

#include "MessageToSend.h"

static void MessageToSend_free(MessageToSend*);

bool MessageToSend_exist(MessageToSend* this) {
	return (this != NULL);
}

void MessageToSend_destroy(MessageToSend* this) {
	if (this != NULL) {
		MessageToSend_free(this);
	}
}

static void MessageToSend_free(MessageToSend* this) {
	free(this);
}


