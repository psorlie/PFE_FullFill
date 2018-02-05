/*
 * bat_level.c
 *
 *  Created on: 14 janv. 2018
 *      Author: pierr
 */

#include "comp.h"
#include <stm32l0xx_hal_comp.h>
#include "gpio.h"

uint32_t bat_measure;

uint8_t bat_level_measure(){
	HAL_COMP_Start(&hcomp1);
	bat_measure = 1;
	bat_measure =  HAL_COMP_GetOutputLevel(&hcomp1);
	HAL_COMP_Stop(&hcomp1);
	return (uint8_t)bat_measure;
}
