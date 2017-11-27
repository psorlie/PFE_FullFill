/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "macro_types.h"
#include "stm32f1_gpio.h"
#include "stm32f1_timer.h"
#include "test.h"
#include "config.h"
#include "adc.h"
#include "math.h"
#include "stm32f1_rtc.h"
#include "sensor.h"
#include "observer.h"




uint8_t ack;
uint8_t highByteVal;
uint8_t lowByteVal;
uint8_t interupt;

void sendData(){
	UART_putc(UART1_ID,highByteVal);
	HAL_Delay(250);
	UART_putc(UART1_ID,lowByteVal);
}
uint8_t listenAck(){
	ack = 0;
	if(UART_data_ready(UART1_ID)){
		ack = UART_get_next_byte(UART1_ID);
		HAL_Delay(100);
		}
	return ack;
}
void toggle_pin(){
	HAL_GPIO_WritePin(LED_GREEN,1);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(LED_GREEN,0);
	HAL_Delay(1000);
}
int main(void)
{
	HAL_Init();
	BSP_GPIO_Enable();
	SYS_ClockConfig();
	UART_init(UART1_ID,115200);
	ADC1_init_for_PA1();

	while(1){
		if(interupt){
			sensorMeasure();
			sendData();
			listenAck();
			while(ack == 0){
				sendData();
				listenAck();
			}
			toggle_pin();
		}

	}

}



