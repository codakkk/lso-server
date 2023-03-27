
#ifndef __H_ROOM__
#define __H_ROOM__

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "client.h"

#define MAX_ROOMS 1000

#define MAX_CLIENTS_PER_ROOM 30
#define MAX_ROOM_NAME 32

extern struct room_t* gRooms[MAX_ROOMS];

typedef struct room_t
{
    client_t* clients[MAX_CLIENTS_PER_ROOM];

    int32_t id;
    int32_t clientsCount;

    int8_t name[MAX_ROOM_NAME];

    client_t* owner;

		pthread_mutex_t mutex;
} room_t;

room_t* room_create(int8_t name[MAX_ROOM_NAME]);
void room_delete(room_t *room);

room_t* room_get(int32_t id);

void room_send_message(room_t* room, message_t* message);
bool room_is_full(room_t* room);

void room_add_client(room_t *room, client_t *client);
bool room_remove_client(room_t *room, client_t *client);

void room_serialize(lso_writer_t* writer, room_t* room);
#endif