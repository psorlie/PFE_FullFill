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

//static void Date_free(Date*);

static void Date_free(Date* this) {
	this->day_of_month = 0;
	this->day_of_year = 0;
	this->month = 0;
	this->year = 0;
	free(this);
}

Date* Date_set_current_date() {
	Date* this;
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	this = (Date *) malloc (sizeof(Date));

	this->year = (1900 + timeinfo->tm_year);
	this->month = timeinfo->tm_mon + 1;
	this->day_of_month = timeinfo->tm_mday;
	this->day_of_year = timeinfo->tm_yday;
	return this;
}

Date* Date_set_specified_date(int day_of_month, int day_of_year, int month, int year) {
	Date* this;
	this = (Date *) malloc (sizeof(Date));

	this->day_of_month = day_of_month;
	this->day_of_year = day_of_year;
	this->month = month;
	this->year = year;

	return this;
}

int Date_get_year(Date* this) {
	assert(this != NULL);
	int ret = this->year;
	assert(ret > 1900);
	return ret;
}

int Date_get_day_of_year(Date* this) {
	assert(this != NULL);
	int ret = this->day_of_year;
	assert(ret >= 0 && ret <= 31);
	return ret;
}

int Date_get_day_of_month(Date* this) {
	assert(this != NULL);
	int ret = this->day_of_month;
	assert(ret >= 0 && ret <= 31);
	return ret;
}

int Date_get_month(Date* this) {
	assert(this != NULL);
	int ret = this->month;
	assert(ret >= 0 && ret <= 12);
	return ret;
}

void Date_destroy(Date *this) {
	if(this != NULL) {
		Date_free(this);
	}
}