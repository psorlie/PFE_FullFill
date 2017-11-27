/*
 * MatrixKeyboard.h
 *
 *  Created on: 24 août 2015
 *      Author: Nirgal
 */

#ifndef LIB_BSP_MATRIXKEYBOARD_MATRIX_KEYBOARD_H_
#define LIB_BSP_MATRIXKEYBOARD_MATRIX_KEYBOARD_H_
#include "stm32f1xx_hal.h"
#include "macro_types.h"


	#define PORT_INPUT_0 	GPIOA
	#define PORT_INPUT_1 	GPIOA
	#define PORT_INPUT_2 	GPIOA
	#define PORT_INPUT_3 	GPIOA
	#define	PIN_INPUT_0		GPIO_PIN_4
	#define	PIN_INPUT_1		GPIO_PIN_5
	#define	PIN_INPUT_2		GPIO_PIN_6
	#define	PIN_INPUT_3		GPIO_PIN_7

	#define PORT_OUTPUT_0 	GPIOA
	#define PORT_OUTPUT_1 	GPIOA
	#define PORT_OUTPUT_2 	GPIOA
	#define PORT_OUTPUT_3 	GPIOA
	#define	PIN_OUTPUT_0	GPIO_PIN_8
	#define	PIN_OUTPUT_1	GPIO_PIN_9
	#define	PIN_OUTPUT_2	GPIO_PIN_10
	#define	PIN_OUTPUT_3	GPIO_PIN_11

	#define MATRIX_KEYBOARD_HAL_CLOCK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()

	#define CONFIG_PULL_UP
	//#define CONFIG_PULL_DOWN


	/*
	@function	Initialise le module keyboard.
	@post		Les ports concernés sont configurés en entrée ou en sortie.
	*/
	void KEYBOARD_init(const char * new_keyboard_keys);

	running_e DEMO_matrix_keyboard_process_main (bool_e ask_for_finish);

	void DEMO_matrix_keyboard_process_1ms(void);


	/*
	@function	Vérifie qu'il y a appui sur une touche ou non.
	@return		TRUE si une touche est appuyée, FALSE sinon.
	*/
	bool_e KEYBOARD_is_pressed(void);



	#define NO_KEY 0xFF
	#define MANY_KEYS 0xFE
	/*
	@function	Renvoi le code ASCII de la touche pressée. En correspondance avec le tableau de codes ASCII.
	@post		Cette fonction intègre un anti-rebond
	@pre		Il est conseillé d'appeler cette fonction périodiquement (10ms par exemple)
	*/
	char KEYBOARD_get_key(void);



#endif /* LIB_BSP_MATRIXKEYBOARD_MATRIX_KEYBOARD_H_ */
