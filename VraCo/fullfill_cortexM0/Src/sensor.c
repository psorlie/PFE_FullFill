/*
 * sensor.c
 *
 *  Created on: 18 nov. 2017
 *      Author: pierr
 */

#include "main.h"
#include "stm32l0xx_hal.h"
#include "adc.h"
#include "sensor.h"

#define GPIO_ACTIVATE_SENSOR GPIOC, GPIO_PIN_14

uint16_t sensorAnalogVal;
uint16_t temp;
uint8_t highByteVal;
uint8_t lowByteVal;
uint8_t tabAnalogSensorVal[2];
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

void sensorMeasure(){
	HAL_GPIO_WritePin(GPIO_ACTIVATE_SENSOR,1);
	HAL_ADC_Start(&hadc);
	HAL_Delay(100);
	sensorAnalogVal =  HAL_ADC_GetValue(&hadc);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIO_ACTIVATE_SENSOR,0);
	HAL_ADC_Stop(&hadc);
	temp = (uint16_t) sensorAnalogVal;
	highByteVal=temp >> 7;
	lowByteVal=(uint8_t)sensorAnalogVal;
	tabAnalogSensorVal[0]=highByteVal;
	tabAnalogSensorVal[1]=lowByteVal;
}


