/*
 * postman.h
 *
 *  Created on: 12 janv. 2018
 *      Author: pierr
 */

#ifndef POSTMAN_H_
#define POSTMAN_H_

/**on récupère la mesure du capteur, on la parse avec l'id et la mesure batterie, et on envoie sur l'UART**/
extern void postman_send(uint8_t, uint8_t,uint8_t);

/**lecture de l'ack sur l'UART**/
void postman_receive(uint32_t);

#endif /* POSTMAN_H_ */
