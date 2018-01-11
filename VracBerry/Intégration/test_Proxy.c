#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "Postman.h"
#include "Proxy.h"
#include <time.h>

int main(int argc, char *argv[])
{

	time_t actTime;
	struct tm *timeComp;
	time(&actTime);
	timeComp = localtime(&actTime);

	Postman_init();

	Postman_listen_start();

	Proxy_send_update(1, 60, "Bouffe");

	char date_string [10];
	sprintf(date_string, "%02d-%02d-%d", timeComp->tm_mday, timeComp->tm_mon + 1, timeComp->tm_year + 1900);
	Proxy_send_detail(2, 60, 30, date_string, "Bouffe");

	char time_string [6];
	sprintf(time_string, "%02d:%02d", timeComp->tm_hour, timeComp->tm_min);
	Proxy_send_network(time_string, time_string, time_string, 15);

	usleep(100000000);

	Postman_listen_stop();
	Postman_destroy();

    return 0;
}