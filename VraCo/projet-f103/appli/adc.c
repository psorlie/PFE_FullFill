/*
 * adc.c
 *
 *  Created on: 22 juin 2016
 *      Author: Samuel Poiraud
 */

#include "config.h"
#include "stm32f1xx_hal.h"
#include "stm32f1_gpio.h"
#include "adc.h"
#include "stm32f1_ili9341.h"

static ADC_HandleTypeDef	adc_handle;							//Handler pour l'ADC.


//Exemple d'utilisation de l'ADC pour un seul canal, sans DMA.
void ADC1_init_for_PA1(void)
{
	//Activation de l'horloge du peripherique ADC1
	__HAL_RCC_ADC_CONFIG(RCC_ADCPCLK2_DIV6);
	__HAL_RCC_ADC1_CLK_ENABLE();

	//Activation de l'horloge du GPIO utilise
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//Appel a la fonction d'initialisation des registres du peripherique GPIO
	GPIO_InitTypeDef GPIO_InitStructure;					//Structure contenant les arguments de la fonction GPIO_Init
	GPIO_InitStructure.Pin = GPIO_PIN_1;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);


	//Initialisation de ADCx
	adc_handle.Instance = ADC1;
	adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;		//Alignement a droite des donnees mesurees dans le resultat sur 16 bits.
	adc_handle.Init.ScanConvMode = DISABLE;					//Mode de conversion : ENABLE si plusieurs canaux. DISABLE si un seul canal.
	adc_handle.Init.ContinuousConvMode = ENABLE;			//Mode continu : ENABLE si mesures en continu. DISABLE si une seule mesure a effectuer.
	adc_handle.Init.NbrOfConversion = 1;					//Nombre de conversions a effectuer dans un cycle de mesure. (minimum 1, maximum 16).
	adc_handle.Init.DiscontinuousConvMode = DISABLE;
	adc_handle.Init.NbrOfDiscConversion = 0;
	adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	HAL_ADC_Init(&adc_handle);								//Initialisation de l'ADCx avec la structure remplie


	//Reglage de l'ADC souhaite sur le canal demande
	ADC_ChannelConfTypeDef sConfig;
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
	HAL_ADC_ConfigChannel(&adc_handle, &sConfig);

	//On lance les conversions (en continu)... C'est parti... (cette fonction comprend l'activation de l'ADCx)
	HAL_ADC_Start(&adc_handle);
}



uint16_t ADC1_get_value(void)
{
	uint16_t ret;
	ret = HAL_ADC_GetValue(&adc_handle);
	return ret;
}

