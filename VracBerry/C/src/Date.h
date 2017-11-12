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

extern bool Date_set_current_date(Date*);

extern bool Date_free(Date*);

#endif /* SRC_DATE_H_ */
