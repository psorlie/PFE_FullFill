/*
 * timerVraco.c
 *
 *  Created on: 9 nov. 2017
 *      Author: pierr
 */

#include "stm32f1_rtc.h"


running_e DEMO_RTC_process_main(bool_e ask_for_finish)
{
	typedef enum
	{
		INIT = 0,
		RUN
	}state_e;
	static state_e state = INIT;
	running_e ret;
	ret = IN_PROGRESS;

	switch(state)
	{
		case INIT: {
			state = RUN;
			RTC_init(FALSE);	//Oscillateur INTERNE, imprécis, mais testable sans ajout d'un oscillateur sur PC14 et PC15 !
			//RTC_init(TRUE);		//Oscillateur EXTERNE, précis, mais nécessite l'ajout d'un oscillateur sur PC14 et PC15 !

			//	1/04/2015	12h34'56"
			//RTC_TimeTypeDef time = {12, 34, 56};
			RTC_TimeTypeDef time = {00, 00, 00};
			RTC_DateTypeDef date = {RTC_WEEKDAY_MONDAY, RTC_MONTH_APRIL, 1, 15};
			RTC_set_time_and_date(&time, &date);
			printf("This demo will print the time every second\n");
			break;}
		case RUN:{
			static uint8_t previous_printed_time = 0;
			RTC_TimeTypeDef time;
			RTC_DateTypeDef date;
			RTC_get_time_and_date(&time, &date);
			if(time.Seconds != previous_printed_time)
			{
				printf("%2d/%2d/%2d - %2d:%2d:%2d\n", date.Date, date.Month, date.Year, time.Hours, time.Minutes, time.Seconds);
				previous_printed_time = time.Seconds;
			}

			if(ask_for_finish)
			{
				state = INIT;
				ret = END_OK;
			}
			break;}
		default:
			break;
	}
	return ret;
}

