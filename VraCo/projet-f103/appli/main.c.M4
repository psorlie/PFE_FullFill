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
#include "timer.h"


#define CAMERA_GREEN_LED	GPIOA, GPIO_PIN_5
#define CAMERA_TRIGGER		GPIOA, GPIO_PIN_6

static volatile uint32_t t = 0;

void state_machine(void);

int main(void)
{
	HAL_Init();					//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer)
	BSP_GPIO_Enable();			//Activation des périphériques GPIO
	SYS_ClockConfig();			//Configuration des horloges.

	UART_init(UART1_ID,115200);	//Initialisation de l'UART1 à la vitesse de 115200 bauds/secondes (92kbits/s) PB6 : Tx  | PB7 : Rx.
	SYS_set_std_usart(UART1_ID, UART1_ID, UART1_ID);	//"Indique que les printf sortent vers le périphérique UART1."

	//Initialisation du port de la led Verte (carte Nucleo)
	BSP_GPIO_PinCfg(LED_GREEN,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	BSP_GPIO_PinCfg(CAMERA_TRIGGER,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port du bouton bleu (carte Nucleo)
	BSP_GPIO_PinCfg(BLUE_BUTTON,GPIO_MODE_INPUT,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);



	while(1)
	{
		state_machine();
	}
}


/*
 * @brief Machine à état.
 * @pre : doit être appelée régulièrement dans la tâche de fond de l'application
 */
void state_machine(void)
{
	typedef enum
	{
		INIT,
		WAIT_BUTTON,
		BLINK_500MS,
		BLINK_100MS,
		RUN_TRIGGER,
		WAIT_10MS
	}state_e;
	static state_e state = INIT;

	switch(state)
	{
		case INIT:
			//Initialisation du Timer 3.
			TIMER3_init_for_1ms();
			TIMER3_run();
			HAL_GPIO_WritePin(CAMERA_GREEN_LED, FALSE);
			HAL_GPIO_WritePin(CAMERA_TRIGGER, FALSE);
			state = WAIT_BUTTON;
			break;
		case WAIT_BUTTON:
			if(!HAL_GPIO_ReadPin(BLUE_BUTTON))
			{
				t = 3000;
				state = BLINK_500MS;
			}
			break;
		case BLINK_500MS:
			HAL_GPIO_WritePin(CAMERA_GREEN_LED, (t/500)%2);
			if(!t)
			{
				t = 2000;
				state = BLINK_100MS;
			}
			break;
		case BLINK_100MS:
			HAL_GPIO_WritePin(CAMERA_GREEN_LED, (t/100)%2);
			if(!t)
				state = RUN_TRIGGER;
			break;
		case RUN_TRIGGER:
			HAL_GPIO_WritePin(CAMERA_TRIGGER, 1);
			t = 10;
			state = WAIT_10MS;
			break;
		case WAIT_10MS:
			if(!t)
			{
				HAL_GPIO_WritePin(CAMERA_TRIGGER, FALSE);
				state = WAIT_BUTTON;
			}
			break;
		default:
			break;
	}
}



void TIMER3_user_handler_it_1ms(void)
{
	if(t)
		t--;
}




