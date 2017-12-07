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

static void Proxy_Application_year_generator(char* , Dispenser* , bool);

static void Proxy_Application_day_generator(char* , Dispenser* , bool);

/**
 * @brief static function that can transform an uint8_t in a "json-adapted" array of char
 *
 * @param[in] the number to transform
 *
 * @retval the char pointer directed to the char array
 */
static char* Proxy_Application_uint8ToChar(uint8_t);

static char* Proxy_Application_intToChar(int);

/**
 * @brief static function that can concatenate any number of char array
 *
 * @param[in] the number of char array
 * @param[in] all the pointer to all the char array
 *
 * @retval a char pointer, directed to the char array containing all the inputed ones
 */
static char* Proxy_Application_concat(int count, ...);

/**
 * @brief static function that can generate a json-adapted line from 4 different char*
 *
 * @param[in] the output
 * @param[in] the first_arg : type of the attribute
 * @param[in] the second arg : what the attribute is
 * @param[in] the third arg : a comma or nothing, depending of if this is the last attributed
 *
 * @retval void
 */
static void Proxy_Application_sprintf(char*, char*, char*, char*);

/**
 * @brief static function that can generate a json-adapted line that give the type of the message sent
 *
 * @param[in] the output
 * @param[in] the first_arg : type of the message
 * @param[in] the second arg : boolean that indicate if this is the last argument of the json object
 *
 * @retval void
 */
static void Proxy_Application_type_generator(char* , char* , bool);

/**
 * @brief static function that can generate the endline a json argument,
 * if it's the last argument, its a \0, else it's a comma
 *
 * @param[in] boolean which is true if it's the last argument of the object
 *
 * @retval char* directing to the line-finisher
 */
static char* Proxy_Application_end_line_generator(bool);

/**
 * @brief static function that can generate a json-adapted line to indicate the filling level of a dispenser
 *
 * @param[in] the output
 * @param[in] the first_arg : Dispenser* about whom we want the filling
 * @param[in] the second arg : boolean that indicate if this is the last argument of the json object
 *
 * @retval void
 */
static void Proxy_Application_filling_generator(char* , Dispenser* , bool);

/**
 * @brief static function that can generate a json-adapted line to indicate the battery level of a dispenser
 *
 * @param[in] the output
 * @param[in] the first_arg : Dispenser* about whom we want the battery
 * @param[in] the second arg : boolean that indicate if this is the last argument of the json object
 *
 * @retval void
 */
static void Proxy_Application_battery_generator(char* , Dispenser* , bool);

/**
 * @brief static function that can generate a json-adapted line to indicate the id of a dispenser
 *
 * @param[in] the output
 * @param[in] the first_arg : Dispenser* about whom we want the id
 * @param[in] the second arg : boolean that indicate if this is the last argument of the json object
 *
 * @retval void
 */
static void Proxy_Application_id_generator(char* , Dispenser* , bool);

static void Proxy_Application_product_name_generator(char* , Dispenser* , bool);

static void Proxy_Application_date_generator(char* , Dispenser* , bool);


void Application_display_all_dispenser(Dispenser* first_dispenser) {
	while(first_dispenser != NULL){
		Application_update_dispenser(first_dispenser);
		first_dispenser = first_dispenser->next_dispenser;
	}
}

void Application_update_dispenser(Dispenser* this) {
	char* prepare_string = (char*)malloc(1000);
	char data[5][100];
	Proxy_Application_type_generator(data[0], "\"update\"", false);
	Proxy_Application_id_generator(data[1], this, false);
	Proxy_Application_filling_generator(data[2], this, false);
	Proxy_Application_battery_generator(data[3], this, false);
	Proxy_Application_product_name_generator(data[4], this, true);
	prepare_string = Proxy_Application_concat(7, "{\"data\":{", data[0], data[1], data[2], data[3], data[4], "}}");
	Postman_send(prepare_string);
}

void Application_detailed_dispenser(Dispenser* this) {
	char* prepare_string = (char*)malloc(1000);
	char data[6][100];
	Proxy_Application_type_generator(data[0], "\"update\"", false);
	Proxy_Application_id_generator(data[1], this, false);
	Proxy_Application_filling_generator(data[2], this, false);
	Proxy_Application_battery_generator(data[3], this, false);
	Proxy_Application_date_generator(data[4], this, false);
	Proxy_Application_product_name_generator(data[5], this, true);
	prepare_string = Proxy_Application_concat(8, "{\"data\":{", data[0], data[1], data[2], data[3], data[4], data[5], "}}");
	Postman_send(prepare_string);
}



// extern void Application_set_network_configuration(Network*);

// extern void Application_set_dispenser_configuration();

//TODO
/*
void Application_send_detailed_dispenser(Dispenser* this) {

}

void Application_warn_dirty_dispenser(Dispenser_Id id){

}*/

static char* Proxy_Application_uint8ToChar(uint8_t value) {
	int size = floor(log10(value))+3;
	char *str = NULL;
	str = (char *)malloc(size + 1);
	memset(str, '\0', (size + 1));
    sprintf(str, "\"%d\"", value);
    return str;
}

static char* Proxy_Application_intToChar(int value) {
	int size = floor(log10(value))+3;
	char *str = NULL;
	str = (char *)malloc(size + 1);
	memset(str, '\0', (size + 1));
    sprintf(str, "\"%d\"", value);
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

static char* Proxy_Application_end_line_generator(bool is_last) {
	if(is_last){
			return "\0";
		} else {
			return ",";
		}
}

static void Proxy_Application_filling_generator(char* to_write, Dispenser* this, bool is_last) {
	char* end_line = Proxy_Application_end_line_generator(is_last);
	char* filling = Proxy_Application_uint8ToChar(Dispenser_get_filling(this));
	Proxy_Application_sprintf(to_write, "\"filling\"", filling, end_line);
	free(filling);
}

static void Proxy_Application_battery_generator(char* to_write, Dispenser* this, bool is_last) {
	char* end_line = Proxy_Application_end_line_generator(is_last);
	char* battery = Proxy_Application_uint8ToChar(Dispenser_get_battery(this));
	Proxy_Application_sprintf(to_write, "\"battery\"", battery, end_line);
	free(battery);
}

static void Proxy_Application_id_generator(char* to_write, Dispenser* this, bool is_last) {
	char* end_line = Proxy_Application_end_line_generator(is_last);
	char* id = Proxy_Application_uint8ToChar(Dispenser_get_id(this));
	Proxy_Application_sprintf(to_write, "\"id\"", id, end_line);
	free(id);
}

static void Proxy_Application_type_generator(char* to_write, char* type, bool is_last) {
	char* end_line = Proxy_Application_end_line_generator(is_last);
	Proxy_Application_sprintf(to_write, "\"type\"", type, end_line);
}

static void Proxy_Application_product_name_generator(char* to_write, Dispenser* this, bool is_last) {
	char* end_line = Proxy_Application_end_line_generator(is_last);
	char* product_name = Dispenser_get_product_name(this);
	char* encapsulated_product_name = Proxy_Application_concat(3, "\"", product_name, "\"");
	Proxy_Application_sprintf(to_write, "\"name\"", encapsulated_product_name, end_line);
}

static void Proxy_Application_date_generator(char* to_write, Dispenser* this, bool is_last) {
	char tab[2][100];
	char* end_line = Proxy_Application_end_line_generator(is_last);
	Proxy_Application_year_generator(tab[0], this, false);
	Proxy_Application_day_generator(tab[1], this, true);
	sprintf(to_write, "%s%s%s%s%s", "\"date\":{", tab[0], tab[1], "}", end_line);
}

static void Proxy_Application_year_generator(char* to_write, Dispenser* this, bool is_last) {
	char* end_line = Proxy_Application_end_line_generator(is_last);
	char* year = Proxy_Application_intToChar(Dispenser_get_year(this));
	Proxy_Application_sprintf(to_write, "\"year\"", year, end_line);
	free(year);
}

static void Proxy_Application_day_generator(char* to_write, Dispenser* this, bool is_last) {
	char* end_line = Proxy_Application_end_line_generator(is_last);
	char* day = Proxy_Application_intToChar(Dispenser_get_day(this));
	Proxy_Application_sprintf(to_write, "\"day\"", day, end_line);
	free(day);
}
