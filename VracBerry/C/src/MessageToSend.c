/*
 * MessageToSend.c
 *
 *  Created on: 16 nov. 2017
 *      Author: sorlie
 */
#include <stdbool.h>
#include <stdlib.h>

#include "MessageToSend.h"

bool MessageToSend_exist(MessageToSend* this) {
	return (this != NULL);
}
