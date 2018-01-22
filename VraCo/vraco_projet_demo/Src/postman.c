/*
 * postman.c
 *
 *  Created on: 12 janv. 2018
 *      Author: pierr
 */

#define GPIO_XBEE_SLEEP GPIOA, GPIO_PIN_4

#include <stdbool.h>
#include "gpio.h"
#include "usart.h"
#include "bat_level.h"
#include "observer.h"
#include "stm32l0xx_hal_gpio.h"
#include <stdlib.h>
#include <string.h>

void postman_send(uint8_t id, uint8_t measure,uint8_t bat_level){

	//uint8_t slash = '/';
	//tabAnalogSensorVal[0] = 0b10100010;
	//tabAnalogSensorVal[1] = 0b10001001;
	//HAL_UART_Transmit(&hlpuart1, tabAnalogSensorVal, 1, 0);
	//HAL_Delay(500);

	char* full_char = (char*) malloc (10);
	HAL_Delay(50);
	memset(full_char, 0, 10);
	sprintf(full_char,"[%c/%c/%c]", id, measure, bat_level);
	printf("coucou : %s, taille du texte : %d\n",full_char, strlen(full_char));
	HAL_UART_Transmit(&hlpuart1, (uint8_t*)full_char, strlen(full_char), 10);
	HAL_Delay(5000);
	free(full_char);

	HAL_GPIO_WritePin(GPIO_XBEE_SLEEP,0);
/*	HAL_Delay(50);
	HAL_UART_Transmit(&hlpuart1, &id, 1, 0);
	HAL_Delay(500);
	HAL_UART_Transmit(&hlpuart1, &slash, 1, 0);
	HAL_Delay(500);
	HAL_UART_Transmit(&hlpuart1, &measure, 1, 0);
	HAL_Delay(500);
	HAL_UART_Transmit(&hlpuart1, &slash, 1, 0);
	HAL_Delay(500);
	HAL_UART_Transmit(&hlpuart1, &bat_level, 1, 0);
	HAL_Delay(50);*/

}
void postman_receive(uint32_t delay) {
	uint8_t* bufferAck = NULL;
	bufferAck = (uint8_t*) malloc(20);
	memset(bufferAck, 0, 20);
	HAL_StatusTypeDef result;
	result = HAL_UART_Receive(&hlpuart1, bufferAck, 8, delay);

	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIO_XBEE_SLEEP,1);
	//result = HAL_OK;
	//snprintf(bufferAck,
	switch (result) {
	case HAL_TIMEOUT:
		observer_has_received_nothing();
		break;
	case HAL_OK:
		//uint8_t size = strlen((char*)bufferAck);
		// malloc la zone sur une grande surface
		//snprintf(bufferAck, size, "%s", bufferAck);
		observer_has_received((char*)bufferAck);
		break;
	default:
		break;
	//perror("[POSTMAN] - Error while receiving something");
	}
}

