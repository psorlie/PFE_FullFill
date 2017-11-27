/*
 * stm32f1_rtc.h
 *
 *  Created on: 21 août 2015
 *      Author: Nirgal
 */

#ifndef LIB_BSP_STM32F1_RTC_H_
#define LIB_BSP_STM32F1_RTC_H_
#include "stm32f1xx_hal.h"
#include "macro_types.h"
#include "config.h"
#if USE_RTC
	//______________________________________________

	/**	running_e DEMO_RTC_process_main(bool_e ask_for_finish)	 // Fonction de démonstration (partielle) de l'utilisation de ce module.
	 * @brief exemple d'utilisation du module RTC.
	 */
	running_e DEMO_RTC_process_main(bool_e ask_for_finish);

	/**
	 * @brief initialise la RTC
	 * @func void RTC_init(bool_e i_installed_an_external_oscillator)
	 * @param i_installed_an_external_oscillator :
	 * 					pour utiliser la RTC, vous devez relier un quartz de 32768Hz entre PC14 et PC15.
	 * 						Si c'est fait, indiquez TRUE
	 * 					Sinon, il est possible d'utiliser l'oscillateur interne : indiquez FALSE
	 * 					Attention, cet oscillateur est médiocre (pourcentage de déviation de plusieurs pourcents !)
	 * 						mais il est pratique pour valider le concept...
	 */
	void RTC_init(bool_e i_installed_an_external_oscillator);

	/**
	 * @brief Used to set the time and date of the RTC module
	 * @param sTime: new time to set
	 * @param sDate: new date to set
	 * @retval none
	 */
	void RTC_set_time_and_date(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate);

	/**
	 * @brief Retrieve the current RTC values of time and Date (in the given sTime and sDate)
	 * @param sTime: adress of the structure where new time will be retrieved
	 * @param sDate: adress of the structure where new date will be retrieved
	 * @retval none
	 */
	void RTC_get_time_and_date(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate);

	/**
	 * @brief Used to set the time and date of the RTC module
	 * @param sAlarm: Alarm to set
	 * @retval none
	 */
	void RTC_set_alarm(RTC_AlarmTypeDef *sAlarm);

	/**
	 * @brief Retrieve the alarm of the RTC
	 * @param sAlarm: Alarm used
	 * @param Alarm: Value of the Alarm
	 * @retval none
	 */
	void RTC_get_alarm(RTC_AlarmTypeDef *sAlarm, uint32_t Alarm);
#endif

#endif /* LIB_BSP_STM32F1_RTC_H_ */
