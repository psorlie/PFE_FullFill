//#include "server.h"
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "Postman.h"

char* message = "truc bidule chouette";

int main(int argc, char *argv[])
{
	//printf("début du main\n");
	Postman_init();

	Postman_listen_start();

	//printf("avant la boucle\n");

	while(1){
		//printf("dans la boucle\n");
    	usleep(1000000);
    	Postman_send(message);
	}

	//printf("Listen end\n");

	Postman_listen_stop();
	Postman_destroy();

	//printf("fin du programme\n");

    return 0;
}

