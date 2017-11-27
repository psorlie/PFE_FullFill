/*
 * sensor.c
 *
 *  Created on: 6 nov. 2017
 *      Author: pierr
 */

#include "sensor.h"
#include "stdint.h"
#include "adc.h"

uint16_t sensorAnalogVal;
uint16_t temp;
uint8_t highByteVal;
uint8_t lowByteVal;
unsigned char str[2];
//running_e ret;


void sensorMeasure(){
	sensorAnalogVal = ADC1_get_value();
	temp = sensorAnalogVal;
	highByteVal=temp >> 8;
	lowByteVal=(uint8_t)sensorAnalogVal;
	str[0]=highByteVal;
	str[1]=lowByteVal;
}


