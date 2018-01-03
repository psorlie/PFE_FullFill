#include "server.h"
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>


char* message = "truc bidule chouette";

void send_topic2(char* topic2){
	int i = 0;
	for(i=0;i<20;i++){
		//printf("dans la boucle\n");
    	send_message(message, topic2);
    	usleep(500000);
    }
	//}
}

int main(int argc, char *argv[])
{

	char* topic1 = "coulis de fraises";
	char* topic2 = "blabla";
	//printf("début du main\n");
	mqtt_init();

	/* this variable is our reference to the second thread */
	pthread_t send_topic2_thread;
	pthread_t receive_thread;

	/* create a second thread which executes inc_x(&x) */
	if(pthread_create(&send_topic2_thread, NULL, send_topic2, topic2)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}

/*
	if(pthread_create(&receive_thread, NULL, listen_thread, NULL)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	*/

	listen_thread();

	//printf("avant la boucle\n");

	printf("Envoi sujet 1\n");

	//while(1){
		//printf("dans la boucle\n");
    	usleep(1000000);
    	send_message(message, topic1);
	//}


	printf("Thread sujet 2 end\n");

	/* wait for the second thread to finish */
	if(pthread_join(send_topic2_thread, NULL)) {
		fprintf(stderr, "Error joining thread 2\n");
		return 2;
	}

	printf("Listen end\n");

	listen_stop();

	printf("Thread réception end\n");

	/* wait for the third thread to finish */
	/*
	if(pthread_join(listen_thread, NULL)) {
		fprintf(stderr, "Error joining thread 3\n");
		return 2;
	}*/

	printf("fin du programme\n");

    return 0;
}

