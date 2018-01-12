#include <stdio.h>
#include <string.h>
#include <mosquitto.h>
#include <unistd.h>
#include "server.h"


struct mosquitto *mosq = NULL;

void send_message(char* message, char* topic)
{
    //printf("dans la callback");
    mosquitto_publish(mosq, NULL, topic, strlen(message), message, 1, false);
}

void listen_thread(){
    //mosquitto_loop_read(mosq, 1);
    mosquitto_loop_start(mosq);
}

void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    printf("réception de message : ");
    if(message->payloadlen){
        printf("%s %s\n", message->topic, (char *)message->payload);
    }else{
        printf("%s (null)\n", message->topic);
    }
    fflush(stdout);
}

void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    //int i;
    if(!result){
        /* Subscribe to broker information topics on successful connect. */
        mosquitto_subscribe(mosq, NULL, "blabla", 1);
        mosquitto_subscribe(mosq, NULL, "coulis de fraises", 1);
        //mosquitto_subscribe(mosq, NULL, "$SYS/#", 2);
    }else{
        fprintf(stderr, "Connect failed\n");
    }
}

void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    int i;

    printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
    for(i=1; i<qos_count; i++){
        printf(", %d", granted_qos[i]);
    }
    printf("\n");
}

void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    /* Print all log messages regardless of level. */
    printf("%s\n", str);
}

void mqtt_init()
{

    //printf("mosquitto init beginnig\n");
    //int i;
    char *host = "localhost";
    int port = 1883;
    int keepalive = 60;
    bool clean_session = true;

    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, clean_session, NULL);
    if(!mosq){
        fprintf(stderr, "Error: Out of memory.\n");
    }
    mosquitto_log_callback_set(mosq, my_log_callback);
    mosquitto_connect_callback_set(mosq, my_connect_callback);
    mosquitto_message_callback_set(mosq, my_message_callback);
    mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);

    if(mosquitto_connect(mosq, host, port, keepalive)){
        fprintf(stderr, "Unable to connect.\n");
    }

    //mosquitto_publish(mosq, NULL, topic1, 12, "miammm slurp", 0, false);
    //usleep(1);

    //printf("mosquitto init end\n");
    
    //mosquitto_loop_start(mosq);

    //mosquitto_loop_forever(mosq, -1, 1);

    //mosquitto_destroy(mosq);
    //mosquitto_lib_cleanup();
}

void listen_stop()
{
    mosquitto_loop_stop(mosq, true);
}

void mqtt_stop(){
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}