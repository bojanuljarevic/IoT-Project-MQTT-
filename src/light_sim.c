/*
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2017/2018
    Semestar:       Zimski (V)

    Ime fajla:      client.c
    Opis:           TCP/IP klijent

    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include <stdio.h>      //printf
#include <stdlib.h>
#include <string.h>     //strlen
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close
#include <mosquitto.h>
#include <time.h>
#include "./lssdp/lssdp.h"

#define SERVER_PORT   1883
#define SERVER_ADDR "192.168.4.1"
#define KEEP_ALIVE 60
#define QOS_1 1
#define TOPIC "light"
#define ONOFF "/on-off/light"

typedef struct {
	char *msg;
	unsigned int len;
} payload;

static int on = 1;


void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);

int show_neighbor_list(lssdp_ctx * lssdp);
int show_interface_list_and_rebind_socket(lssdp_ctx * lssdp);
void log_callback(const char * file, const char * tag, int level, int line, const char * func, const char * message);
 

void delay(int number_of_seconds) 
{ 
    // Converting time into milli_seconds 
    int milli_seconds = 1000 * number_of_seconds; 
  
    // Storing start time 
    clock_t start_time = clock(); 
  
    // looping till required time is not achieved 
    while (clock() < start_time + milli_seconds); 
} 


lssdp_ctx lssdp = {
        // .debug = true,           // debug
        .port = 1900,
        .neighbor_timeout = 1000,  // 15 seconds
        .header = {
            .search_target       = "ST_P2P",
            .unique_service_name = "f835dd000001",
            .sm_id               = "700000123",
            .device_type         = "LIGHT_SENSOR",
            .location.suffix     = ":5677"
        },
        // callback
        .neighbor_list_changed_callback     = show_neighbor_list,
        .network_interface_changed_callback = show_interface_list_and_rebind_socket,
    };
  

int main(int argc , char *argv[])
{	
	
	mosquitto_lib_init();
	struct mosquitto *mq = mosquitto_new("light1", true, NULL);
	if(!mq){
		fprintf(stderr, "Error: Failed to initiate mosquitto client.\n");
		return 1;
	}
		
	
	if(mosquitto_connect(mq, SERVER_ADDR, SERVER_PORT, KEEP_ALIVE)){
		fprintf(stderr, "Unable to connect.\n");
		return 1;
	}
	
	payload pld;
	unsigned int num = 0;
	sprintf(pld.msg, "%d", num);
	pld.len = strlen(pld.msg);
	srand(time(0)); 
	
	
	lssdp_set_log_callback(log_callback);
	lssdp_network_interface_update(&lssdp);
		
	
	mosquitto_subscribe(mq, NULL, ONOFF, QOS_1);	
	mosquitto_message_callback_set(mq, message_callback);
		
	
	
	while(1) {
		
		mosquitto_loop(mq, -1, 1); 
		
		if(on) {		
			
			mosquitto_publish(mq, NULL, TOPIC, pld.len, pld.msg, QOS_1, false);
			lssdp_send_notify(&lssdp);
					
		}
		
		delay(1000);
		int x = rand() % 100;
		if(x > 80) num++;
		else if (x > 60) {
			if(num != 0) {
				num--;
			}
		}
		sprintf(pld.msg, "%d", num);
		
	}
	
		
	mosquitto_destroy(mq);
	mosquitto_lib_cleanup();
	

    return 0;
}



void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{	
	
	bool match = 0;
	printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);

	mosquitto_topic_matches_sub("/on-off/+", message->topic, &match);
	if (match) {
		printf("got message for ADC topic\n");
	}
	
	
	char onStr[1];
	strcpy(onStr, (char*)message->payload);
		
	if(onStr[0] == '1') {
		on = 1;
	} else {
		on = 0;
	}

}


int show_neighbor_list(lssdp_ctx * lssdp) {
    int i = 0;
    lssdp_nbr * nbr;
    puts("\nSSDP List:");
    for (nbr = lssdp->neighbor_list; nbr != NULL; nbr = nbr->next) {
        printf("%d. id = %-9s, ip = %-20s, name = %-12s, device_type = %-8s (%lld)\n",
            ++i,
            nbr->sm_id,
            nbr->location,
            nbr->usn,
            nbr->device_type,
            nbr->update_time
        );
    }
    printf("%s\n", i == 0 ? "Empty" : "");
    return 0;
}

int show_interface_list_and_rebind_socket(lssdp_ctx * lssdp) {
    // 1. show interface list
    printf("\nNetwork Interface List (%zu):\n", lssdp->interface_num);
    size_t i;
    for (i = 0; i < lssdp->interface_num; i++) {
        printf("%zu. %-6s: %s\n",
            i + 1,
            lssdp->interface[i].name,
            lssdp->interface[i].ip
        );
    }
    printf("%s\n", i == 0 ? "Empty" : "");

    // 2. re-bind SSDP socket
    if (lssdp_socket_create(lssdp) != 0) {
        puts("SSDP create socket failed");
        return -1;
    }

    return 0;
}

void log_callback(const char * file, const char * tag, int level, int line, const char * func, const char * message) {
    char * level_name = "DEBUG";
    if (level == LSSDP_LOG_INFO)   level_name = "INFO";
    if (level == LSSDP_LOG_WARN)   level_name = "WARN";
    if (level == LSSDP_LOG_ERROR)  level_name = "ERROR";

    printf("[%-5s][%s] %s", level_name, tag, message);
}


