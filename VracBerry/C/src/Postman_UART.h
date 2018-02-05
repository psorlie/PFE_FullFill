
/*
 * MessageToSend.h
 *
 *  Created on: 14 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_POSTMAN_UART_H_
#define SRC_POSTMAN_UART_H_

#define LENGTH_RECEIVED_STRING	(7) // currently 7, can change ?

/**
 * @brief This function is called at the beginning of the program, to initialise some variable and
 * create the thread that will listen to the xbee module.
 */
extern void Postman_UART_init();

/**
 * @brief This function is called when the VracBerry wants to send something to a dispenser.
 * @legacy_code, it probably works alone, but in a multithread environnement, it will probably go wrong.
 *
 * @param[in] char*, pointer to the text to send (it also doesn't free the text)
 */
extern void Postman_UART_send(char*);

/**
 * @brief This function is called when we want to stop the listening on the UART. It just do that, so care about
 * the thread trying to listening to a close UART.
 */
extern void Postman_UART_close();

#endif
