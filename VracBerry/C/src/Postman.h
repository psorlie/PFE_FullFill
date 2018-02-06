/*
 * Postman.h
 *
 *  Created on: 29 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_POSTMAN_H_
#define SRC_POSTMAN_H_

/******************************** MQTT broker ******************************/

/**
 * @def quality of service
 * 0 -> no ACK ;	 1 -> 1 ACK ; 	2 -> 4-stage handshake
 *
 */
#define QOS				1 					

/**
 * @def name of the MQTT topic to send messages to the web interface
 *
 */
#define TOPIC_SEND		"c2js"
 /**
 * @def name of the MQTT topic to receive messages to the web interface
 *
 */
#define TOPIC_RECEIVE	"js2c"				// UI sending to VracBerry

/**
 * @def IP adress where the MQTT broker sends / receive messages --> change to 192.168.50.1 for the Raspberry
 *
 */
#define HOST 			"localhost" 		// IP address of the broker

 /**
 * @def port where the MQTT broker sends / receive messages for the C code
 *
 */
#define PORT 			1883

 /**
 * @def maximum connection time for the MQTT broker (sec) --> a ping is sent every KEEP_ALIVE seconds to keep the session alive
 *
 */
#define KEEP_ALIVE 		60

 /**
 * @def boolean : erase the topic subscriptions after disconnect?
 *
 */
#define CLEAN_SESSION 	true

 /**
 * @def boolean : erase the messages after disconnect? (if RETAIN = FALSE, be carefulto erase the messages regularely)
 *
 */
#define RETAIN			false

/******************************** JSON messages ******************************/

 /**
 * @def received when the web interface chages screen for the detailed window
 *
 */
#define TYPE_ASK_DETAIL		"ask_detail"

 /**
 * @def received when the web interface chages screen for the main screen
 *
 */
#define TYPE_ASK_UPDATE		"ask_full_update"

 /**
 * @def received when the web interface chages screen for the network window
 *
 */
#define TYPE_ASK_NETWORK	"ask_network"

 /**
 * @def received when the user signaled that a dispenser has been cleaned today
 *
 */
#define TYPE_CLEAN_UPDATE	"cleaning_update"

 /**
 * @def received when the user changed the name of a dispenser
 *
 */
#define TYPE_NAME_UPDATE	"name_update"

 /**
 * @def received when the user changed the network configuration
 *
 */
#define TYPE_NETWORK_UPDATE	"network_update"

/**
 * @brief function to initialize the MQTT client
 *
 */
extern void Postman_init();

/**
 * @brief function to destroy the MQTT client
 *
 */
extern void Postman_destroy();

/**
 * @brief function to start the listenning thread
 *
 */
extern void Postman_listen_start();

/**
 * @brief function to stop the listenning thread
 *
 */
extern void Postman_listen_stop();

/**
 * @brief function to send a message to the web interface through the MQTT broker
 * 
 * @param message to send
 *
 */
extern void Postman_send(char* message);

#endif /* SRC_POSTMAN_H_ */
