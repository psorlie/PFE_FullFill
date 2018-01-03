/*
 * Postman.h
 *
 *  Created on: 29 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_POSTMAN_H_
#define SRC_POSTMAN_H_

/******************************** libmosquitto ******************************/

#define QOS				1 				// 0 -> no ACK ;	 1 -> 1 ACK ; 	2 -> 4-stage handshake

#define TOPIC_C2JS		"c2js" 		// VracBerry sending to UI
#define TOPIC_JS2C		"js2c"		// UI sending to VracBerry

#define HOST 			"localhost" 	// IP address of the broker
#define PORT 			1883 			// MQTT port of the broker
#define KEEP_ALIVE 		60 				// Maximum connection time
#define CLEAN_SESSION 	true			// FALSE -> keep topic subscriptions after disconnect ; 	TRUE -> don't keep subscriptions


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
