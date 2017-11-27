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

volatile uint32_t t_ms = 0;
volatile bool_e flag_new_value = FALSE;

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
	BSP_GPIO_PinCfg(BLUE_BUTTON,GPIO_MODE_IT_RISING_FALLING,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
	HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI15_10_IRQn));

	//Initialisation du Timer 3.
	TIMER3_init_for_1ms();

	while(1)
	{
		if(flag_new_value) 			//Lorsque le drapeau est levé
		{
			flag_new_value = FALSE;	//On baisse le drapeau
			printf("%lu\n",t_ms);	//On affiche l’information…
		}
	}
}



void EXTI15_10_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != RESET)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);		//On acquitte l’it
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == 0)	//Front descendant - le bouton est appuyé
		{
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
			t_ms = 0; 			//RAZ compteur
			TIMER3_run();		//On lance le timer
		}
		else											//Front montant - le bouton est relâché
		{
			TIMER3_stop();			//On arrête le timer
			flag_new_value = TRUE; 	//On lève le flag
		}
	}
}


void TIMER3_user_handler_it_1ms(void)
{
	t_ms++;
}




