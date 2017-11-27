/*
 * timer.c
 *
 *  Created on: 23 juin 2016
 *      Author: Nirgal
 */
#include "timer.h"
#include "stm32f1xx_hal.h"

static TIM_HandleTypeDef tim_handle;

void TIMER3_init_for_1ms(void)
{
	__HAL_RCC_TIM3_CLK_ENABLE();

	// On fixe les priorités des interruptions du timer3 PreemptionPriority = 0, SubPriority = 1 et on autorise les interruptions
	HAL_NVIC_SetPriority(TIM3_IRQn , 0, 1);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);

	// Time base configuration
	tim_handle.Instance 			= TIM3; //On donne le timer 3 en instance à notre gestionnaire (Handle)
	tim_handle.Init.Prescaler 		= 64 - 1;	//le prescaler du timer doit être enregistré avec un offset de -1.
	tim_handle.Init.Period 			= 1000 - 1;	//On compte de 0 à period-1
	tim_handle.Init.ClockDivision 	= TIM_CLOCKDIVISION_DIV1;
	tim_handle.Init.CounterMode 	= TIM_COUNTERMODE_UP;

	// On applique les paramètres d'initialisation
	HAL_TIM_Base_Init(&tim_handle);

	// On autorise les interruptions
	HAL_TIM_Base_Start_IT(&tim_handle);
}

void TIMER3_run(void)
{
	// On lance le timer
	__HAL_TIM_ENABLE(&tim_handle);
}

void TIMER3_stop(void)
{
	__HAL_TIM_DISABLE(&tim_handle);
}

//Routine d'interruption appelée automatiquement lorsque le timer arrive à échéance.
void TIM3_IRQHandler(void)
{
	if(__HAL_TIM_GET_IT_SOURCE(&tim_handle, TIM_IT_UPDATE) != RESET)
	{
		__HAL_TIM_CLEAR_IT(&tim_handle, TIM_IT_UPDATE); //Acquittement
		TIMER3_user_handler_it_1ms();		//Appel de la fonction de l'utilisateur.
	}
}

