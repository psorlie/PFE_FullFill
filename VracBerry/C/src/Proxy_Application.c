/*
 * Proxy_Application.c
 *
 *  Created on: 19 nov. 2017
 *      Author: sorlie
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Application.h"
#include "DispenserManager.h"
#include "Postman.h"

#define MAXIMUM_MESG_SIZE (1024)

static void Application_send_dispenser(Dispenser*);
static char* Proxy_Application_intToChar(uint8_t);
static char* Proxy_Application_concat(int count, ...);
static void Proxy_Application_sprintf(char*, char*, char*, char*);

static void Application_send_dispenser(Dispenser* this) {

}

void Application_display_all_dispenser(Dispenser* first_dispenser) {
	while(first_dispenser != NULL){
		Application_send_dispenser(first_dispenser);
		first_dispenser = first_dispenser->next_dispenser;
	}
}

void Application_update_dispenser(Dispenser_Id id){
	int i = 0;
	Dispenser* this = DispenserManager_find_dispenser(id);
	char* prepare_string = (char*)malloc(1000);
	char* tableau_de_char[8];
	tableau_de_char[0] = "\"type\"";
	tableau_de_char[1] = "\"update\"";
	tableau_de_char[2] = "\"id\"";
	tableau_de_char[3] = Proxy_Application_intToChar(id);
	tableau_de_char[4] = "\"filling\"";
	uint8_t transitoire = DispenserManager_get_filling(id);
	tableau_de_char[5] = Proxy_Application_intToChar(transitoire);
	tableau_de_char[6] = "\"battery\"";
	transitoire = (uint8_t)DispenserManager_get_battery(id);
	tableau_de_char[7] = Proxy_Application_intToChar(transitoire);
	char data[4][100];
	for (i = 0 ; i < 4 ; i++) {
		char* virgule;
		if(i<3) {
			virgule = ",";
		} else {
			virgule = "\0";
		}
		Proxy_Application_sprintf(data[i], tableau_de_char[2*i], tableau_de_char[(2*i)+1], virgule);
	}
	prepare_string = Proxy_Application_concat(6, "{", data[0], data[1], data[2], data[3], "}");
	Postman_send(prepare_string);
	printf("\nsortie test pour envoi json : %s", prepare_string);
}

// extern void Application_set_network_configuration(Network*);

// extern void Application_set_dispenser_configuration();

void Application_warn_dirty_dispenser(Dispenser_Id id){

}

void Application_warn_empty_dispenser(Dispenser_Id id) {

}

void Application_warn_empty_battery(Dispenser_Id id) {

}

static char* Proxy_Application_intToChar(uint8_t value){
	int size = floor(log10(value))+1;
	char *str = NULL;
	str = (char *)malloc(size + 1);
	memset(str, '\0', (size + 1));
    sprintf(str, "%d", value);
    return str;
}

static char* Proxy_Application_concat(int count, ...) {
    va_list ap;
    int i;

    // Find required length to store merged string
    int len = 1; // room for NULL
    va_start(ap, count);
    for(i=0 ; i<count ; i++){
    	len += strlen(va_arg(ap, char*));
    }
    va_end(ap);

    // Allocate memory to concat strings
    char *merged = calloc(sizeof(char),len);
    int null_pos = 0;

    // Actually concatenate strings
    va_start(ap, count);
    for(i=0 ; i<count ; i++)
    {
        char *s = va_arg(ap, char*);
        strcpy(merged+null_pos, s);
        null_pos += strlen(s);
    }
    va_end(ap);

    return merged;
}

static void Proxy_Application_sprintf(char* output, char* first_arg, char* second_arg, char* third_arg){
	sprintf(output, "%s:%s%s",first_arg, second_arg, third_arg);
}
