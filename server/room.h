
#ifndef __H_ROOM__
#define __H_ROOM__

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "./client.h"

#define MAX_CLIENTS_PER_ROOM 30

struct room_t {
    pthread_mutex_t mutex;
    pthread_t tid;

    struct client_t* clients[MAX_CLIENTS_PER_ROOM];

    int32_t id;
    int32_t clientsCount;

    char channelName[32];
};

struct room_t* room_create(char name[32]) ;

void room_lock(struct room_t* room);
void room_unlock(struct room_t* room);

void room_add_client(struct room_t *room, struct client_t *cl);
void room_remove_client(struct room_t *room, struct client_t* uid);
int room_count_clients(struct room_t* room);

void _room_try_matches(struct room_t* room);

// Called when two clients get matched
void _on_match(struct room_t* room, struct client_t* c1, struct client_t* c2);

// Called when a client leaves the room
void _on_leave_room(struct room_t* room, struct client_t* client);

// Called when a new client joins the room
void _on_enter_room(struct room_t* room, struct client_t* client);

void* room_update(void* arg);
#endif