/*
 * Translator.c
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#include "Translator.h"

void Translator_send_message(uint8_t id, char* message) {
	printf("message pour %d - %s", id, message);
}
