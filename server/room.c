#include "room.h"
#include "client.h"
#include "tags.h"
#include "messages.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int counter_room = 0;
room_t *gRooms[MAX_ROOMS];

int room_find_client_index(room_t *room, client_t *client)
{
	pthread_mutex_lock(&room->mutex);
  int index = -1;
  for (int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
  {
    client_t *roomClient = room->clients[i];
    if (roomClient == NULL || roomClient->uid != client->uid)
      continue;
    index = i;
    break;
  }
	pthread_mutex_unlock(&room->mutex);
  return index;
}

room_t* room_create(int8_t name[MAX_ROOM_NAME])
{
  room_t* room = (room_t*) malloc(sizeof(room_t));
  room->id = counter_room++;

  for(int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
  {
    room->clients[i] = NULL;
  }

  room->clientsCount = 0;
  strcpy(room->name, name);
  
  gRooms[room->id] = room;

	pthread_mutex_init(&room->mutex, NULL);
  return room;
}

void room_delete(room_t *room)
{
  if (room == NULL)
  {
    return;
  }

  gRooms[room->id] = NULL;

  // TODO(gio): remove all clients from room

  free(room);
}

room_t* room_get(int32_t id)
{
  return gRooms[id];
}

void room_send_message(room_t* room, message_t* message)
{
	pthread_mutex_lock(&room->mutex);
	for(int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
  {
    client_t* roomClient = room->clients[i];
    
    if(roomClient == NULL) 
    {
      continue;
    }

    client_send(roomClient, message);
  }
	pthread_mutex_unlock(&room->mutex);
}

bool room_is_full(room_t* room)
{
  if(room == NULL)
  {
    printf("UNDEFINED BEHAVIOUR: Called room_is_full on NULL Room.\n");
    return false;
  }

  if(room->clientsCount == MAX_CLIENTS_PER_ROOM)
  {
    return true;
  }

  return false;
}

/**
 * Adds @client to @room
 * Returns -1 if room is full, >= 0 otherwise
 * 
*/
void room_add_client(room_t *room, client_t *client)
{
	int index = -1;
	bool alreadyPresent = false;
	for (int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
	{
		client_t* roomClient = room->clients[i];
		if (roomClient == NULL)
		{
				index = i;
		}
		else if(roomClient->user.id == client->user.id)
		{
			alreadyPresent = true;
		}
	}

  // Room is full
  if (index == -1 || (alreadyPresent && client->room->id == room->id))
  {
    return;
  }

	pthread_mutex_lock(&room->mutex);

  room->clients[index] = client;
  room->clientsCount++;

	pthread_mutex_unlock(&room->mutex);

	pthread_mutex_lock(&client->mutex);
  client->room = room;
	pthread_mutex_unlock(&client->mutex);

  printf("Client id %d has joined the room %s\n", client->uid, room->name);
}

/* Remove client from room */
bool room_remove_client(room_t *room, client_t *client)
{
  if (room == NULL)
  {
    return false;
  }

  int index = room_find_client_index(room, client);
  if (index == -1)
  {
    if(client->room == room)
    {
      client->room = NULL;
    }
    return false;
  }

	pthread_mutex_lock(&room->mutex);
  room->clients[index] = NULL;
  room->clientsCount--;
	pthread_mutex_unlock(&room->mutex);

	pthread_mutex_lock(&client->mutex);
  client->room = NULL;
	pthread_mutex_unlock(&client->mutex);
  return true;
}

void room_serialize(lso_writer_t* writer, room_t* room)
{
	pthread_mutex_lock(&room->mutex);
  lso_writer_write_int32(writer, room->id);
  lso_writer_write_int32(writer, room->clientsCount);
  lso_writer_write_int32(writer, MAX_CLIENTS_PER_ROOM);
  lso_writer_write_string(writer, room->name, strlen(room->name));
  client_serialize(room->owner, writer);

  for(int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
  {
    client_t* roomClient = room->clients[i];

    if(roomClient == NULL) 
    {
      continue;
    }

    client_serialize(roomClient, writer);
  }

	pthread_mutex_unlock(&room->mutex);
}
