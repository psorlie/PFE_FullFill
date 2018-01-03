#ifndef __SERVER_H
#define __SERVER_H

#include <stdint.h>

void mqtt_init();
void listen_stop();

void send_message(char* message, char* topic);

void listen_thread();


#endif /*__SERVER_H */
