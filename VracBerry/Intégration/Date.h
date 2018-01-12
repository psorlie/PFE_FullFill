/*
 * Date.h
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_DATE_H_
#define SRC_DATE_H_

#include <stdbool.h>

typedef struct {
	int year;
	int current_day;
}Date;

extern Date* Date_set_current_date();

extern void Date_destroy(Date*);

extern int Date_get_year(Date*);

extern int Date_get_month(Date* this);

extern int Date_get_day(Date*);

extern Date* Date_set_specified_date(int, int);

#endif /* SRC_DATE_H_ */
