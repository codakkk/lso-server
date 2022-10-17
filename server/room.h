
#ifndef __H_ROOM__
#define __H_ROOM__

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "client.h"
#include "chat.h"

#define MAX_ROOMS 10

#define MAX_CLIENTS_PER_ROOM 30
#define MAX_ROOM_NAME 32
#define MAX_CHAT_PER_ROOM 50

extern struct room_t* gRooms[MAX_ROOMS];

typedef struct room_t {
    pthread_mutex_t mutex;
    pthread_t tid;

    struct client_t* clients[MAX_CLIENTS_PER_ROOM];

    int32_t id;
    int32_t clientsCount;

    char channelName[MAX_ROOM_NAME];

    chat_t* chats[MAX_CHAT_PER_ROOM];
} room_t;

room_t* room_create(char name[MAX_ROOM_NAME]);

void room_lock(room_t* room);
void room_unlock(room_t* room);

bool room_try_join(room_t* room, client_t* client);
bool room_leave(room_t* room, client_t* client);

void _room_try_matches(room_t* room);
//

// Called when two clients get matched
void _on_match(room_t* room, client_t* c1, client_t* c2);

// Called when a client leaves the room
void _on_leave_room(room_t* room, client_t* client);

// Called when a new client joins the room
void _on_enter_room(room_t* room, client_t* client);

void* room_update(void* arg);
#endif