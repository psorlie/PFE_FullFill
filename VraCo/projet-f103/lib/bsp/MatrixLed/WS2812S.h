/*
 * WS2812S.h
 *
 *  Created on: 3 mai 2016
 *      Author: Nirgal
 */

#ifndef BSP_MATRIXLED_WS2812S_H_
#define BSP_MATRIXLED_WS2812S_H_

#include "config.h"

#if !(defined LED_MATRIX_PORT_DATA) || !defined(LED_MATRIX_PIN_DATA)
	#define LED_MATRIX_PORT_DATA	GPIOB
	#define LED_MATRIX_PIN_DATA		GPIO_PIN_13
#endif
void LED_MATRIX_init(void);

void LED_MATRIX_test(void);

//@pre pixels est un tableau de 64 cases absolument...
void LED_MATRIX_display(uint32_t pixels[64]);
void LED_MATRIX_send_pixel(uint32_t pixel);
void LED_MATRIX_reset(void);


#endif /* BSP_MATRIXLED_WS2812S_H_ */
