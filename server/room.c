#include "room.h"
#include "client.h"
#include "tags.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int counter_room = 0;
room_t *gRooms[MAX_ROOMS];

int find_index(room_t *room)
{
  int index = -1;
  for (int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
  {
    if (!room->clients[i])
    {
      index = i;
      break;
    }
  }
  return index;
}

int find_client_index(room_t *room, client_t *client)
{
  int index = -1;
  for (int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
  {
    client_t *roomClient = room->clients[i];
    if (roomClient == NULL || roomClient->uid != client->uid)
      continue;
    index = i;
    break;
  }
  return index;
}

room_t* room_create(char name[32])
{
  room_t* room = (room_t *)malloc(sizeof(room_t));
  room->id = counter_room++;

  pthread_mutex_init(&room->mutex, NULL);
  pthread_create(&room->tid, NULL, &room_update, (void *)room);

  strcpy(room->name, name);
  
  gRooms[room->id] = room;

  return room;
}

void room_delete(room_t *room)
{
  if (room == NULL)
  {
    return;
  }

  gRooms[room->id] = NULL;

  // TODO: remove all clients from room

  free(room);
}

void room_lock(room_t *room)
{
  pthread_mutex_lock(&room->mutex);
}

void room_unlock(room_t *room)
{
  pthread_mutex_unlock(&room->mutex);
}

/* Add clients to room */
bool room_try_join(room_t *room, client_t *client)
{
  int index = find_index(room);

  // Room is full
  if (index == -1)
  {
    return false;
  }

  room_lock(room);

  room->clients[index] = client;
  room->clientsCount++;

  client->room = room;

  room_unlock(room);

  printf("Client id %d has joined the room %s\n", client->uid, room->name);
  return true;
}

/* Remove client from room */
bool room_leave(room_t *room, client_t *client)
{
  if (room == NULL)
  {
    return false;
  }

  int index = find_client_index(room, client);
  if (index == -1)
  {
    if(client->room == room)
    {
      client->room = NULL;
    }
    return false;
  }

  room_lock(room);

  room->clients[index] = NULL;
  room->clientsCount--;

  room_unlock(room);

  client->room = NULL;

  printf("Notifing clients that client id %d left the room.\n", client->uid);

  lso_writer_t writer;
  lso_writer_initialize(&writer, 1);
  lso_writer_write_string(&writer, client->user->name);
  message_t *message = message_create_from_writer(kLeaveRoomTag, &writer);

  for(int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
  {
    if(room->clients[i] == NULL) {
      continue;
    }

    client_send(room->clients[i], message);
  }
  
  message_delete(message);


  printf("Client id %d left room %s.\n", client->uid, room->name);

  return true;
}

void *room_update(void *arg)
{
  room_t *room = (room_t *)arg;

  while (1)
  {
    sleep(1);
  }

  room_delete(room);
}

void room_serialize(lso_writer_t* writer, room_t* room)
{
  lso_writer_write_int32(writer, room->id);
  lso_writer_write_int32(writer, room->clientsCount);
  lso_writer_write_int32(writer, MAX_CLIENTS_PER_ROOM);
  lso_writer_write_string(writer, room->name);
}