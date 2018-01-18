#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "DispenserManager.h"
#include <time.h>
#include "Postman.h"

int main(){
	DispenserManager_init();
	//DispenserManager_add_dispenser(1, "Banania", 52, 65);
	//DispenserManager_printf("");
	Postman_init();
	Postman_listen_start();
	DispenserManager_add_dispenser(1, "Banania", 52, 65);
	usleep(100);
	DispenserManager_received_message(1,20,10);
	DispenserManager_add_dispenser(2, "Bloups", 52, 65);
	usleep(100);
	DispenserManager_received_message(2,0,10);
	DispenserManager_add_dispenser(3, "Scronch", 52, 65);
	usleep(100);
	DispenserManager_received_message(3,20,10);
	DispenserManager_add_dispenser(4, "Cratsch", 52, 65);
	usleep(100);
	DispenserManager_received_message(4,20,0);
	DispenserManager_add_dispenser(5, "Miam", 52, 65);
	usleep(100);
	DispenserManager_received_message(5,20,10);
	DispenserManager_add_dispenser(6, "Vroum", 52, 65);
	usleep(100);
	DispenserManager_received_message(6,20,10);

	while(1){

	}

	return 0;
}
