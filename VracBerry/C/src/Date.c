/*
 * Date.c
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "Date.h"

bool Date_free(Date* this) {
	this->current_day = 0;
	this->year = 0;
	free(this);
	return true;
}

Date* Date_set_current_date() {
	Date* this;
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	this = (Date *) malloc (sizeof(Date));

	this->year = (1900 + timeinfo->tm_year);
	this->current_day = timeinfo->tm_yday;
	return this;
}

int Date_get_year(Date* this) {
	assert(this != NULL);
	int ret = this->year;
	assert(ret > 1900);
	return ret;
}

int Date_get_day(Date* this) {
	assert(this != NULL);
	int ret = this->current_day;
	assert(ret >= 0 && ret <= 366);
	return ret;
}
