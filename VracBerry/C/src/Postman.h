/*
 * Postman.h
 *
 *  Created on: 29 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_POSTMAN_H_
#define SRC_POSTMAN_H_

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

#endif /* SRC_POSTMAN_H_ */
