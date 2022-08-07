
#ifndef __H_ROOM__
#define __H_ROOM__

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "./client.h"

#define MAX_CLIENTS_PER_ROOM 30
pthread_mutex_t room_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int id;
    client_t* clients[MAX_CLIENTS_PER_ROOM];
    
    pthread_t tid;

    char channelName[32];
} room_t;

int counter_room = 0;

room_t* create_room(char name[32]) ;
void add_client_room(room_t *room, client_t *cl);
void remove_client_room(room_t *room, int uid);
int count_in_room(room_t* room);

void handle_chat();


void* handle_room(void* arg);

#include "./room.c"
#endif