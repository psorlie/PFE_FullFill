/*
 * Translator.h
 *
 *  Created on: 12 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_TRANSLATOR_H_
#define SRC_TRANSLATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * @brief This function is called to send a text from the postman_uart to the dispenserManager.
 *
 * @param[in] char*, pointer to the text received by the UART.
 */
extern void Translator_send_to_server(char*);

#endif /* SRC_TRANSLATOR_H_ */
