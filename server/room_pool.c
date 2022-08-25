#include "room_pool.h"
#include "room.h"

#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct room_pool_t room_pool;

void room_pool_initialize()
{
}

int get_room_pool_empty_index()
{
  int index = -1;

  for(int i = 0; i < MAX_ROOMS; ++i) 
  {
    if(room_pool.rooms[i] != NULL) continue;
    index = i;
    break;
  }

  return index;
}

bool room_pool_add(struct room_t* room)
{
  if(room == NULL)
  {
    return false;
  }

  int index = get_room_pool_empty_index();

  if(index == -1) 
  {
    perror("ERROR(room_pool_add): unable to add new room. Aborting.\n");
    return false;
  }

  room_pool.rooms[index] = room;
  room_pool.size++;
  return true;
}

void room_pool_send_all(struct client_t* client)
{
  char buff_out[4096];

  client_send_message(client, "Select your room:\n");

  for (int i = 0; i < MAX_ROOMS; i++)
  {
    struct room_t* room = room_pool.rooms[i];
    if(room == NULL) continue;
    
    sprintf(buff_out, "%d) %s %d/%d.\n", i, room->channelName, room->clientsCount, MAX_CLIENTS_PER_ROOM);
    client_send_message(client, buff_out);
  }
}

struct room_t* room_pool_get_by_index(int index)
{
  if(index < 0 || index >= MAX_ROOMS) return NULL;

  return room_pool.rooms[index];
}

void room_pool_list_all()
{
  char buff_out[4096];

  for (int i = 0; i < MAX_ROOMS; i++)
  {
    struct room_t* room = room_pool.rooms[i];
    if(room == NULL) continue;
    printf("%d) %s %d/%d\n", i, room->channelName, room->clientsCount, MAX_CLIENTS_PER_ROOM);
  }
}