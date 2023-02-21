
#ifndef __H_ROOM__
#define __H_ROOM__

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "client.h"

#define MAX_ROOMS 100

#define MAX_CLIENTS_PER_ROOM 30
#define MAX_ROOM_NAME 32

extern struct room_t *gRooms[MAX_ROOMS];

typedef struct room_t
{
    pthread_mutex_t mutex;
    pthread_t tid;

    struct client_t *clients[MAX_CLIENTS_PER_ROOM];

    int32_t id;
    int32_t clientsCount;

    char channelName[MAX_ROOM_NAME];
} room_t;

room_t *room_create(char name[MAX_ROOM_NAME]);
void room_delete(room_t *room);

void room_lock(room_t *room);
void room_unlock(room_t *room);

bool room_try_join(room_t *room, client_t *client);
bool room_leave(room_t *room, client_t *client);

void *room_update(void *arg);
#endif