#ifndef SRC_PROXY_H_
#define SRC_PROXY_H_

#include <stdint.h>

#include "Dispenser.h"

void Proxy_send_update(Dispenser*);

void Proxy_send_new(Dispenser*);

void Proxy_send_detail(Dispenser*);

/*
void Proxy_send_update(uint8_t id, uint8_t filling, char* name);

void Proxy_send_detail(uint8_t id, uint8_t filling, uint8_t battery, char* date, char* name);

void Proxy_send_network(char* sleep_time, char* wake_up_time, uint8_t cycle_time_min, uint8_t cleaning_interval_day);
*/

#endif /* SRC_PROXY_H_ */