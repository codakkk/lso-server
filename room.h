
#ifndef __H_ROOM__
#define __H_ROOM__

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "./client.h"

#define MAX_CLIENTS_PER_ROOM 30

typedef struct {
    int id;
    client_t* clients[MAX_CLIENTS_PER_ROOM];
    
    pthread_t tid;

    char channelName[32];
} room_t;

int counter = 0;

room_t* create_room(client_t* client1, client_t* client2, char name[32]) ;
int add_client(room_t* room, client_t* client);
int find_empty_index(room_t* room);

void* handle_room(void* arg);


#endif