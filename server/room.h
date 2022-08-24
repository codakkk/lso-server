
#ifndef __H_ROOM__
#define __H_ROOM__

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "./client.h"

#define MAX_CLIENTS_PER_ROOM 30

struct room_t {
    pthread_mutex_t mutex;
    int id;
    struct client_t* clients[MAX_CLIENTS_PER_ROOM];
    
    pthread_t tid;

    char channelName[32];
};

struct room_t* room_create(char name[32]) ;
void _room_start_chat(struct client_t* client);
void room_add_client(struct room_t *room, struct client_t *cl);
void room_remove_client(struct room_t *room, struct client_t* uid);
int room_count_clients(struct room_t* room);
void handle_chat();


void* room_update(void* arg);
#endif