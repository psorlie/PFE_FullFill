// Dans ce fichier, il faut modifier les callbacks d'envoi et de réception des données ainsi que le main

#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mqueue.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stddef.h>
#include <time.h>

#include "Postman.h"

static struct lws *web_socket = NULL;

static pthread_t postman_thread;


#define EXAMPLE_RX_BUFFER_BYTES (4)

static PostmanMqMsg Postman_mq_receive();

static void Postman_mq_send(PostmanMqMsg);

static void* Postman_run();

static void Postman_mq_send(PostmanMqMsg msg) {
	int check;
	PostmanMqMsgAdapter msg_adapter;
	mqd_t mail_queue;
	msg_adapter.data = msg;
	mail_queue = mq_open(MQ_POSTMAN_NAME, O_WRONLY);
	if(mail_queue <= -1) {
		perror("[Postman] - Couldn\'t open the postman MQ\n");
		strcpy(msg_adapter.buffer, "\0\0");
	} else {
		check = mq_send(mail_queue, msg_adapter.buffer, sizeof(msg_adapter.buffer), 0);
		if (check <= -1) {
			perror("[Postman] - Couldn\'t write in the MQ\n");
		}
	}
	check = mq_close(mail_queue);
}


static PostmanMqMsg Postman_mq_receive() {
	struct timespec default_mq_reading_timeout;
	clock_gettime(CLOCK_REALTIME, &default_mq_reading_timeout);
	default_mq_reading_timeout.tv_sec += 2;
	int check;
	mqd_t mail_queue;
	PostmanMqMsgAdapter msg_adapter;
	mail_queue = mq_open(MQ_POSTMAN_NAME, O_RDONLY);
	if(mail_queue == -1) {
		perror("[Postman] - Couldn\'t open the postman MQ\n");
		strcpy(msg_adapter.buffer,"\0\0");
	} else {
		check = mq_timedreceive(mail_queue, msg_adapter.buffer, MQ_POSTMAN_MSG_SIZE, 0, &default_mq_reading_timeout);
		if(check == -1 || check == ETIMEDOUT) {
			perror("[Postman] - MQ is empty\n");
			strcpy(msg_adapter.buffer,"\0\0");
		}
	}
	check = mq_close(mail_queue);
	return msg_adapter.data;
}

static int callback_example( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
	switch( reason )
	{
		// Etablissement de la connexion
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			lws_callback_on_writable( wsi );
			break;

		// Réception des données
		case LWS_CALLBACK_CLIENT_RECEIVE:
			// Ici va le JSON parse pour les données recçues.
			// Il doit exister une fonction lws_read un peu comme lws_write( wsi, p, n, LWS_WRITE_TEXT );
			break;

		// Envoi de données
		case LWS_CALLBACK_CLIENT_WRITEABLE:
		{
			PostmanMqMsg msg;
			printf("je me rends compte que j'ai un truc à envoyer\n");
			msg = Postman_mq_receive();
			size_t size_of_sending_string = sizeof((char*) msg.text_pointer);
			printf("\n does it work ? %s \n", msg.text_pointer);
			lws_write( wsi, size_of_sending_string, msg.text_pointer, LWS_WRITE_TEXT );
			free(msg.text_pointer);
			break;
		}

		case LWS_CALLBACK_CLOSED:
			web_socket = NULL;
			break;
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			web_socket = NULL;
			break;

		default:
			perror("Some issue with lws ? don't know\n");
			break;
	}

	return 0;
}

enum protocols
{
	PROTOCOL_EXAMPLE = 0,
	PROTOCOL_COUNT
};

static struct lws_protocols protocols[] =
{
	{
		"example-protocol",
		callback_example,
		0,
		EXAMPLE_RX_BUFFER_BYTES,
	},
	{ NULL, NULL, 0, 0 } /* terminator */
};

static void* Postman_run() {
	/******************** Patouche à cette section ************************/
	struct lws_context_creation_info info;
	memset( &info, 0, sizeof(info) );

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
	info.gid = -1;
	info.uid = -1;

	struct lws_context *context = lws_create_context( &info );
	/******************** Fin de la section patouche ************************/

	/******************** A changer en fonction de l'application ************************/
	time_t old = 0;
	while( 1 )
	{
		struct timeval tv;
		gettimeofday( &tv, NULL );

		// On se connecte si ce n'est pas déjà fait
		if( !web_socket && tv.tv_sec != old )
		{
			// Changer l'adresse et le port d'écoute si nécessaire
			struct lws_client_connect_info ccinfo = {0};
			ccinfo.context = context;
			ccinfo.address = "localhost";
			ccinfo.port = 8000;
			ccinfo.path = "/";
			ccinfo.host = lws_canonical_hostname( context );
			ccinfo.origin = "origin";
			ccinfo.protocol = protocols[PROTOCOL_EXAMPLE].name;
			web_socket = lws_client_connect_via_info(&ccinfo);
		}
		// Go à chaque fois que le timeout est passé
		if( tv.tv_sec != old )
		{
			// Là on appelle la callback d'envoi au serveur
			lws_callback_on_writable( web_socket );
			old = tv.tv_sec;	// on rafraîchit le timer
		}

		lws_service( context, /* timeout_ms = */ 5000 );	// changer le timeout si nécessaire
	}

	lws_context_destroy( context );

	return 0;
}

void Postman_init() {
	struct mq_attr mail_queue_attributes;
	int check;
	mqd_t mail_queue;
	mail_queue_attributes.mq_maxmsg = MQ_POSTMAN_MSG_COUNT;
	mail_queue_attributes.mq_msgsize = MQ_POSTMAN_MSG_SIZE;
	mail_queue_attributes.mq_flags = 0;

	check = mq_unlink(MQ_POSTMAN_NAME);
	if(check != 0) {
		perror("[Postman] - MQ already exist ?\n");
	}

	mail_queue = mq_open(MQ_POSTMAN_NAME, O_CREAT, 0777, &mail_queue_attributes);
	if (mail_queue <= -1) {
		perror("[Postman] - Couldn\' open the MQ during init\n");
	}

	check = mq_close(mail_queue);
	if (check != 0) {
		perror("[Postman] - Issue while closing the MQ during init\n");
	}
	check = pthread_create(&postman_thread, NULL, (void*)&Postman_run, NULL);
	if (check != 0) {
		perror("[Postman] - Issue while creating postman thread\n");
	}
}

void Postman_send(char* text_to_send) {
	PostmanMqMsg msg;
	msg.text_pointer = text_to_send;
	printf("dans le send : %s\n", msg.text_pointer);
	Postman_mq_send(msg);
}
