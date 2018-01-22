/*
 * Translator.c
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#include "Translator.h"


void Translator_send_to_server(char* message){				// message : [id/filling/battery] 1 caractère par donnée
	int id, filling, battery;
	sscanf(message, "[%d/%d/%d]", &id, &filling, &battery);
	printf("FINAL : %d, %d, %d\n", id, filling, battery);
	DispenserManager_add_dispenser(Dispenser_Id, "Bloup", Battery, Filling);
}
