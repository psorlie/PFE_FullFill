/*
 * Postman.h
 *
 *  Created on: 29 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_POSTMAN_H_
#define SRC_POSTMAN_H_

/******************************** libmosquitto ******************************/

#define QOS				1 					// 0 -> no ACK ;	 1 -> 1 ACK ; 	2 -> 4-stage handshake

#define TOPIC_SEND		"c2js"	 			// VracBerry sending to UI
#define TOPIC_RECEIVE	"js2c"				// UI sending to VracBerry

#define HOST 			"localhost" 		// IP address of the broker
#define PORT 			1883 				// MQTT port of the broker
#define KEEP_ALIVE 		60 					// Maximum connection time
#define CLEAN_SESSION 	true				// FALSE -> keep topic subscriptions after disconnect
#define RETAIN			false 				// TRUE -> keep messages in the memory (be careful tu erase it after reception)

#define TYPE_ASK_DETAIL		"ask_detail"
#define TYPE_ASK_UPDATE		"ask_full_update"
#define TYPE_ASK_NETWORK	"ask_network"
#define TYPE_CLEAN_UPDATE	"cleaning_update"
#define TYPE_NAME_UPDATE	"name_update"
#define TYPE_NETWORK_UPDATE	"network_update"


extern void Postman_init();
extern void Postman_destroy();

// Listenning thread
extern void Postman_listen_start();
extern void Postman_listen_stop();

// Sending task
extern void Postman_send(char* message);


/******************************** libwebsockets ******************************/

/*
#define MQ_POSTMAN_MSG_SIZE (1024)
#define MQ_POSTMAN_MSG_COUNT (10)

#define MQ_POSTMAN_NAME "/mq_postman"

typedef struct {
	char* text_pointer;
} PostmanMqMsg;

typedef union {
	char buffer[MQ_POSTMAN_MSG_SIZE];
	PostmanMqMsg data;
} PostmanMqMsgAdapter;


extern void Postman_send(char*);
extern void Postman_init();
*/

#endif /* SRC_POSTMAN_H_ */
