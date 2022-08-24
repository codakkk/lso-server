#include "room_pool.h"
#include "room.h"

#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct room_pool_t pool;


int get_room_pool_empty_index()
{
  pthread_mutex_lock(&pool.mutex);

  int index = -1;

  for(int i = 0; i < MAX_ROOMS; ++i) 
  {
    if(pool.rooms[i] != NULL) continue;
    index = i;
    break;
  }

  pthread_mutex_unlock(&pool.mutex);

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

  pthread_mutex_lock(&pool.mutex);

  pool.rooms[index] = room;
  pool.size++;

  pthread_mutex_unlock(&pool.mutex);
  return true;
}

void room_pool_send_all(struct client_t* client)
{
  char buff_out[2048];

  sprintf(buff_out, "Select your room:\n");
  client_send_message(client, buff_out);

  pthread_mutex_lock(&pool.mutex);

  for (int i = 0; i < MAX_ROOMS; i++)
  {
      struct room_t* room = pool.rooms[i];
      if (room)
      {
          sprintf(buff_out, "%d) %s %d/%d.\n", i, room->channelName, room_count_clients(room), MAX_CLIENTS_PER_ROOM);
          client_send_message(client, buff_out);
      }
  }

  pthread_mutex_unlock(&pool.mutex);
}

struct room_t* room_pool_get_by_index(int index)
{
  if(index < 0 || index >= MAX_ROOMS) return NULL;

  pthread_mutex_lock(&pool.mutex);

  struct room_t* room = pool.rooms[index];

  pthread_mutex_unlock(&pool.mutex);

  return room;
}