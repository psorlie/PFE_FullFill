/*
 * Proxy_Application.c
 *
 *  Created on: 19 nov. 2017
 *      Author: sorlie
 */

/* NOTE : 
Pour avoir la date et l'heure du jour : 
	time_t actTime;
	struct tm *timeComp;

	time(&actTime);
	timeComp = localtime(&actTime);

	// Possibilité n°1 : 
	printf("%d-%02d-%02d \n", timeComp->tm_year + 1900, timeComp->tm_mon + 1, timeComp->tm_mday);	//date
	printf("%02d:%02d:%02d \n", timeComp->tm_hour, timeComp->tm_min, timeComp->tm_sec);				//heure

	// Possibilité n°2 : 
	char date_string [10];
	sprintf(date_string, "%02d-%02d-%d", timeComp->tm_mday, timeComp->tm_mon + 1, timeComp->tm_year + 1900);

	char time_string [6];
	sprintf(time_string, "%02d:%02d", timeComp->tm_hour, timeComp->tm_min);
*/


//TODO : utilisation de CJSON + création de Proxy_send_...() pour chaque envoi différent + appel de Postman_send()

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//#include "Application.h"
#include "Postman.h"
#include "cJSON.h"
//#include "Dispenser.h"
#include "Proxy.h"
#include <time.h>


void Proxy_send(char* message){
	Postman_send(message);
}

void Proxy_send_update(int id, int filling, char* name){
	cJSON *msg = NULL;
	msg = cJSON_CreateObject();
	cJSON_AddStringToObject(msg, "type", "update");
	cJSON_AddNumberToObject(msg, "id", id);
	cJSON_AddNumberToObject(msg, "filling", filling);
	//cJSON_AddNumberToObject(update, "battery", battery);
	cJSON_AddStringToObject(msg, "name", name);
	Proxy_send(cJSON_Print(msg));
}

void Proxy_send_detail(int id, int filling, int battery, char* date, char* name){

	cJSON *msg = NULL;
	msg = cJSON_CreateObject();
	cJSON_AddStringToObject(msg, "type", "detail");
	cJSON_AddNumberToObject(msg, "id", id);
	cJSON_AddNumberToObject(msg, "filling", filling);
	cJSON_AddNumberToObject(msg, "battery", battery);
	cJSON_AddStringToObject(msg, "date", date);
	cJSON_AddStringToObject(msg, "name", name);
	Proxy_send(cJSON_Print(msg));
}

void Proxy_send_network(char* sleep_time, char* wake_up_time, char* cycle_time, int cleaning_interval_day){

	cJSON *msg = NULL;
	msg = cJSON_CreateObject();
	cJSON_AddStringToObject(msg, "type", "network");
	cJSON_AddStringToObject(msg, "sleep_time", sleep_time);
	cJSON_AddStringToObject(msg, "wake_up_time", wake_up_time);
	cJSON_AddStringToObject(msg, "cycle_time", cycle_time);
	cJSON_AddNumberToObject(msg, "cleaning_interval_day", cleaning_interval_day);
	Proxy_send(cJSON_Print(msg));
}

/*
void Proxy_send_dirty_alert(Dispenser_t[] dispenser_list){
	//TODO
	perror("[Application-Proxy] - On prévient l'application que le dispenser n'est pas nettoyé\n");
}

void Proxy_send_broken_alert(Dispenser_Id id) {
	//TODO
	perror("[Application-Proxy] - On prévient l'application que le dispenser est cassé\n");
}

void Proxy_send_lost_alert(Dispenser_Id id) {
	//TODO
	perror("[Application-Proxy] - On prévient l'application que le dispenser est perdu\n");
}

void Proxy_send_configuration_accepted() {
	//TODO
	perror("[Application-Proxy] - On prévient l'application que la configuration est acceptée\n");
}

void Proxy_send_configuration_refused() {
	//TODO
	perror("[Application-Proxy] - On prévient l'application que la configuration est refusée\n");
}

void Proxy_send_new_dispenser(Dispenser* this) {
	//TODO
	perror("[Application-Proxy] - Rajouter un bouton sur l'IHM + demander le nom du produit\n");
}

*/
