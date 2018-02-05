/*
 * Date.h
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_DATE_H_
#define SRC_DATE_H_

#include <stdbool.h>

/**
 * @def Date : contain the date of last washing in 2 formats :
 * 	- day of month / month / year : example 11/09/2001
 * 	- day of year / year : example 345/2001
 */
typedef struct {
	int day_of_month;
	int day_of_year;
	int year;
	int month;
}Date;

/**
 * @brief This function create a date, fill it with current date and return it.
 *
 * @return Date* of the date
 */
extern Date* Date_set_current_date();

/**
 * @brief This function reset all the value in Date, then free it.
 *
 * @param[in] Date* to destroy
 */
extern void Date_destroy(Date*);

/**
 * @brief This function will return the year in the Date.
 *
 * @param[in] Date* about whom we want the year
 *
 * @return int : the year
 */
extern int Date_get_year(Date*);

/*
 * @brief This function will return the month in the Date.
 *
 * @param[in] Date* about whom we want the month
 *
 * @return int : the month
 */
extern int Date_get_month(Date* this);

/**
 * @brief This function will return the day of the month in the Date.
 *
 * @param[in] Date* about whom we want the day of the month
 *
 * @return int : the day of the month
 */
extern int Date_get_day_of_month(Date*);

/**
 * @brief This function will return the day of the year in the Date.
 *
 * @param[in] Date* about whom we want the day of the year
 *
 * @return int : the day of the year
 */
extern int Date_get_day_of_year(Date*);

/**
 * @brief This function will create a date with the specified parameters (use it with the backup reading function of the DM
 *
 * @param[in] int day of the month
 *
 * @param[in] int day of the year
 *
 * @param[in] int month
 *
 * @param[in] int year
 *
 * @return Date*, pointer to the struct
 */
extern Date* Date_set_specified_date(int, int, int, int);

#endif /* SRC_DATE_H_ */
