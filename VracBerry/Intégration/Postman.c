#include <stdio.h>
#include <string.h>
#include <mosquitto.h>
#include <unistd.h>
#include <stdint.h>
#include <unistd.h>
#include "Postman.h"
#include "cJSON.h"
#include "math.h"
//#include "Proxy.h"
#include "DispenserManager.h"

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
    char* type_str = "";
    if(cJSON_IsString(type)){
        type_str = type -> valuestring;
    } else {
        perror ("message non géré : type pas un string");
    }

    printf("type %s", type_str);
    
    if(strcmp(type_str, TYPE_ASK_DETAIL)==0){
        cJSON* id = cJSON_GetObjectItem(root, "id");
        int id_int = 0;
        if(cJSON_IsNumber(id)){
            id_int = id -> valueint;
        } else {
            perror ("message non géré : id pas un int");
        }

        printf("détail, %d\n", id_int);
        DispenserManager_ask_detailed_dispenser(id_int);
    }
    else if(strcmp(type_str, TYPE_ASK_UPDATE)==0){
        printf("update\n");
        usleep(1000000);
        //Proxy_send_update(2, 40, "schniorf");
        DispenserManager_ask_all_update();
    }
    else if(strcmp(type_str, TYPE_ASK_NETWORK)==0){
        printf("network\n");
        //Proxy_send_network("10:30", "12:30", 30, 31);
        // Appel de la fonction dans Network Manager
    }
    else if(strcmp(type_str, TYPE_CLEAN_UPDATE)==0){
        cJSON* id = cJSON_GetObjectItem(root, "id");
        int id_int = 0;
        if(cJSON_IsNumber(id)){
            id_int = id -> valueint;
        } else {
            perror ("message non géré : id pas un int");
        }

        printf("clean, %d\n", id_int);
        DispenserManager_prepare_set_current_date(id_int);
    }
    else if(strcmp(type_str, TYPE_NAME_UPDATE)==0){
        cJSON* id = cJSON_GetObjectItem(root, "id");
        int id_int = 0;
        if(cJSON_IsNumber(id)){
            id_int = id -> valueint;
        } else {
            perror ("message non géré : id pas un int");
        }

        cJSON* name = cJSON_GetObjectItem(root, "name");
        char* name_str = "";
        if(cJSON_IsString(name)){
            name_str = name -> valuestring;
        } else {
            perror ("message non géré : name pas un string");
        }

        printf("détail, %d, %s\n", id_int, name_str);
        DispenserManager_prepare_set_new_product_name(id_int, name_str);
    }
    else if(strcmp(type_str, TYPE_NETWORK_UPDATE)==0){
        cJSON* sleep_time = cJSON_GetObjectItem(root, "sleep_time");
        char* sleep_time_str = "";
        if(cJSON_IsString(sleep_time)){
            sleep_time_str = sleep_time -> valuestring;
        } else {
            perror ("message non géré : sleep_time pas un string");
        }

        cJSON* wake_up_time = cJSON_GetObjectItem(root, "wake_up_time");
        char* wake_up_time_str = "";
        if(cJSON_IsString(wake_up_time)){
            wake_up_time_str = wake_up_time -> valuestring;
        } else {
            perror ("message non géré : wake_up_time pas un string");
        }

        cJSON* cycle_time = cJSON_GetObjectItem(root, "cycle_time");
        char* cycle_time_str = "";
        if(cJSON_IsString(cycle_time)){
            cycle_time_str = cycle_time -> valuestring;
        } else {
            perror ("message non géré : cycle_time pas un string");
        }
        
        cJSON* cleaning_interval_day = cJSON_GetObjectItem(root, "cleaning_interval_day");
        char* cleaning_interval_day_str = "";
        if(cJSON_IsString(cleaning_interval_day)){
            cleaning_interval_day_str = cleaning_interval_day -> valuestring;
        } else {
            perror ("message non géré : cleaning_interval_day pas un string");
        }
        
        printf("network, %s, %s, %s, %s\n", sleep_time_str, wake_up_time_str, cycle_time_str, cleaning_interval_day_str);
        // Appel de la fonction dans Network Manager
    }
    else
        perror("message non géré : type de JSON inconnu");
}

static void Postman_dispatch(const struct mosquitto_message *message){
    cJSON *msg = NULL;
    msg = cJSON_Parse(message->payload);
    parse_object(msg);
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
    //printf("début d'init");
    mosquitto_lib_init();
    //printf("init lib OK");
    mosq = mosquitto_new(NULL, CLEAN_SESSION, NULL);
    if(!mosq){
        fprintf(stderr, "Error: Out of memory.\n");
    }
    //printf("nouveau mosquitto OK");
    mosquitto_log_callback_set(mosq, my_log_callback);
    mosquitto_connect_callback_set(mosq, my_connect_callback);
    mosquitto_message_callback_set(mosq, my_message_callback);
    mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);
    //printf("callbacks OK");

    // Connection to the broker
    if(mosquitto_connect(mosq, HOST, PORT, KEEP_ALIVE)){
        fprintf(stderr, "Unable to connect.\n");
    }
    //printf("connect OK");
}

// Destruction of the MQTT client
void Postman_destroy(){
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}