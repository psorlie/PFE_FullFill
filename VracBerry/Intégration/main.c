#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "DispenserManager.h"
#include <time.h>
#include "Postman.h"
#include "Postman_UART.h"

int main(){
	DispenserManager_init();
	//DispenserManager_add_dispenser(1, "Banania", 52, 65);
	//DispenserManager_printf("");
	Postman_init();
	Postman_listen_start();

	Postman_UART_init();

	DispenserManager_add_dispenser(1, "Farine", 40, 80);
	//usleep(100);
	//DispenserManager_received_message(1,20,10);
	DispenserManager_add_dispenser(2, "Pistaches", 20, 60);
	//usleep(100);
	//DispenserManager_received_message(2,0,10);
	DispenserManager_add_dispenser(3, "Riz", 50, 10);
	//usleep(100);
	//DispenserManager_received_message(3,20,10);
	DispenserManager_add_dispenser(4, "Pâtes", 70, 20);
	//usleep(100);
	//DispenserManager_received_message(4,20,0);
	DispenserManager_add_dispenser(5, "Lentilles", 100, 100);
	//usleep(100);
	//DispenserManager_received_message(5,20,10);
	DispenserManager_add_dispenser(6, "Millet", 50, 0);
	//usleep(100);
	//DispenserManager_received_message(6,20,10);

	while(1){

	}

	Postman_listen_stop();
	Postman_destroy();

	Postman_UART_close();

	return 0;
	
}
