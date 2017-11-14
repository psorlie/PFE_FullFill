/*
 * Date.c
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#include <stdlib.h>
#include <time.h>

#include "Date.h"


bool Date_set_current_date(Date* this) {
	if(this != NULL){
		free(this);
	}
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	this = (Date *) malloc (sizeof(Date));

	this->year = (1900 + timeinfo->tm_year);
	this->current_day = timeinfo->tm_yday;

	return (this == NULL);
}

bool Date_free(Date* this) {
	free(this);
	return (this == NULL);
}
