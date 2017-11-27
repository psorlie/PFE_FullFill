/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"
#include "adc.h"


int main(void)
{
	HAL_Init();					//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer)
	BSP_GPIO_Enable();			//Activation des périphériques GPIO
	SYS_ClockConfig();			//Configuration des horloges.

	UART_init(UART1_ID,115200);	//Initialisation de l'UART1 à la vitesse de 115200 bauds/secondes (92kbits/s) PB6 : Tx  | PB7 : Rx.
	SYS_set_std_usart(UART1_ID, UART1_ID, UART1_ID);	//"Indique que les printf sortent vers le périphérique UART1."

	//Initialisation du port de la led Verte (carte Nucleo)
	BSP_GPIO_PinCfg(LED_GREEN,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port du bouton bleu (carte Nucleo)
	BSP_GPIO_PinCfg(BLUE_BUTTON,GPIO_MODE_INPUT,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	ADC1_init_for_PA1();

	while(1)
	{
		uint16_t analog_value = ADC1_get_value();
		printf("%d\n",analog_value);
		HAL_Delay(400);
	}
}



