/*
 * sensor.h
 *
 *  Created on: 18 nov. 2017
 *      Author: pierr
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#define NB_SAMPLES (1)


#endif /* SENSOR_H_ */

uint8_t sensor_measure(void);
uint8_t convertDecimalToBinary(int n);
uint8_t sensor_get_old_measure(void);
