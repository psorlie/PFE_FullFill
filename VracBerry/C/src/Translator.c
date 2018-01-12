/*
 * Translator.c
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#include "Translator.h"

void Translator_send_message(uint8_t id, char* message) {
	char perror_message [1000];
	sprintf(perror_message, "message pour %d - %s", id, message);
	perror(perror_message);
}
