#include <stdio.h>
#include <string.h>
#include <mosquitto.h>
#include <unistd.h>
#include <stdint.h>
#include "Postman.h"
#include "cJSON.h"

struct mosquitto *mosq = NULL;

void Postman_send(char* message)
{
    mosquitto_publish(mosq, NULL, TOPIC_SEND, strlen(message), message, QOS, RETAIN);
}

// Start listen thread
void Postman_listen_start(){
    mosquitto_loop_start(mosq);
}

// Stop listen thread
void Postman_listen_stop()
{
    mosquitto_loop_stop(mosq, true);
}

static void parse_object(cJSON *root)
{
    cJSON* type = cJSON_GetObjectItem(root, "type");
    cJSON* id = cJSON_GetObjectItem(root, "id");
    /*
    if(strcmp((char*)type, "")==0){
        ...
    }*/
    printf("type %s ; id %d", (char*)type, (uint8_t)id);
}

static void Postman_dispatch(const struct mosquitto_message *message){
    cJSON *msg = NULL;
    msg = cJSON_Parse(message->payload);
    parse_object_dispenser(msg);
}

// Callback of message reception
static void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    //printf("réception de message : ");
    if(message->payloadlen){
        printf("%s %s\n", message->topic, (char *)message->payload);
        //Dispatcher_dispatch(message->payload);							//TODO : create dispatcher
        Postman_dispatch(message);
    }else{
        printf("%s (null)\n", message->topic);
    }
    fflush(stdout);
}

// Callback called when the mosquitto client connects
static void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    if(!result){
        // Subscribe to broker information topics on successful connect.
        //mosquitto_subscribe(mosq, NULL, TOPIC_C2JS, QOS);
        mosquitto_subscribe(mosq, NULL, TOPIC_RECEIVE, QOS);
        // mosquitto_subscribe(mosq, NULL, "$SYS/#", QOS);		// Subscribe to all broker messages
    }else{
        fprintf(stderr, "Connect failed\n");
    }
}

// Callcack called each time the client subscribes to a new subject
static void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    int i;

    printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
    for(i=1; i<qos_count; i++){
        printf(", %d", granted_qos[i]);
    }
    printf("\n");
}

// Callback for log messages
static void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    // Print all log messages regardless of level.
    printf("%s\n", str);
}

// Initialisation of the MQTT client : config & connection
void Postman_init()
{
	/*
char *host = "localhost";
int port = 1883;
int keepalive = 60;
bool clean_session = true;
*/
    // Mosquitto init
    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, CLEAN_SESSION, NULL);
    if(!mosq){
        fprintf(stderr, "Error: Out of memory.\n");
    }
    mosquitto_log_callback_set(mosq, my_log_callback);
    mosquitto_connect_callback_set(mosq, my_connect_callback);
    mosquitto_message_callback_set(mosq, my_message_callback);
    mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);

    // Connection to the broker
    if(mosquitto_connect(mosq, HOST, PORT, KEEP_ALIVE)){
        fprintf(stderr, "Unable to connect.\n");
    }
}

// Destruction of the MQTT client
void Postman_destroy(){
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}