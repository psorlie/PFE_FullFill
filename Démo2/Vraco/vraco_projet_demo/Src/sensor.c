/*
 * sensor.c
 *
 *  Created on: 18 nov. 2017
 *      Author: pierr
 */

#include <main.h>
#include "stm32l0xx_hal.h"
#include "adc.h"
#include "sensor.h"
#include "stm32l0xx_hal_gpio.h"

#define GPIO_ACTIVATE_SENSOR GPIOC, GPIO_PIN_14


uint16_t sensorAnalogVal;
uint8_t currentRate;
static uint8_t previous_rate;


uint8_t sensor_get_old_measure() {
	return previous_rate;
}

uint8_t sensor_measure() {
	uint16_t total_value = 0;
	for(int i =0; i<NB_SAMPLES; i++){
		HAL_GPIO_WritePin(GPIO_ACTIVATE_SENSOR,1);
		HAL_Delay(1000);
		HAL_ADC_Start(&hadc);
		HAL_Delay(500);
		sensorAnalogVal =  HAL_ADC_GetValue(&hadc);
		HAL_Delay(500);
		HAL_GPIO_WritePin(GPIO_ACTIVATE_SENSOR,0);
		HAL_Delay(500);
		HAL_ADC_Stop(&hadc);
	if(sensorAnalogVal >= 3750)
		currentRate = 100;
	if(sensorAnalogVal >= 3500 && sensorAnalogVal < 3750)
		currentRate = 90;
	if(sensorAnalogVal >= 3250 && sensorAnalogVal < 3500)
		currentRate = 80;
	if(sensorAnalogVal >= 3000 && sensorAnalogVal < 3250)
		currentRate = 70;
	if(sensorAnalogVal >= 2500 && sensorAnalogVal < 3000)
		currentRate = 60;
	if(sensorAnalogVal >= 2000 && sensorAnalogVal < 2500)
		currentRate = 50;
	if(sensorAnalogVal >= 1500 && sensorAnalogVal < 2000)
		currentRate = 40;
	if(sensorAnalogVal >= 1250 && sensorAnalogVal < 1500)
		currentRate = 30;
	if(sensorAnalogVal >= 1000 && sensorAnalogVal < 1250)
		currentRate = 20;
	if(sensorAnalogVal >= 500 && sensorAnalogVal < 1000)
		currentRate = 10;
	if(sensorAnalogVal >= 0 && sensorAnalogVal < 500)
		currentRate = 0;
	total_value += currentRate;
	}
	return (uint8_t)(total_value/NB_SAMPLES);
}


