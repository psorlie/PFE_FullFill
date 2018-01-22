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
uint16_t temp;
uint8_t highByteVal;
uint8_t lowByteVal;
uint8_t tabAnalogSensorVal[2];
uint8_t currentRate;
static uint8_t previous_rate;
//running_e ret;

/*uint8_t convertDecimalToBinary(int n)
{
	uint8_t binaryNumber = 0;
	uint8_t remainder, i = 1;

    while (n!=0)
    {
        remainder = n%2;
        n /= 2;
        binaryNumber += remainder*i;
        i *= 10;
    }
    return binaryNumber;
}*/

/*void getBin(uint8_t num, uint8_t *str)
{
  *(str+5) = '\0';
  uint8_t mask = 0x10 << 1;
  while(mask >>= 1)
    *str++ = !!(mask & num) + '0';
}*/

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



	/*temp = (uint16_t) sensorAnalogVal;
	highByteVal=temp >> 7;
	lowByteVal=(uint8_t)sensorAnalogVal;
	tabAnalogSensorVal[0]=highByteVal;
	tabAnalogSensorVal[1]=lowByteVal;*/


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

	//tabAnalogSensorVal[0]=previous_rate;
	//tabAnalogSensorVal[1]=currentRate;
	//previous_rate=currentRate;
	total_value += currentRate;
	}
	return (uint8_t)(total_value/NB_SAMPLES);
}


