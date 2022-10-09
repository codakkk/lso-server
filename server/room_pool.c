#include "room_pool.h"
#include "room.h"
#include "utils.h"
#include "tags.h"

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

struct room_t* room_pool_get_by_index(int index)
{
  if(index < 0 || index >= MAX_ROOMS) return NULL;

  return room_pool.rooms[index];
}

struct room_t* room_pool_get_by_id(int32_t id)
{
  for(int i = 0; i < MAX_ROOMS; ++i)
  {
    struct room_t* room = room_pool.rooms[i];
    if(room == NULL || room->id != id) continue;
    return room;
  }
  return NULL;
}

message_t* create_rooms_message()
{
  lso_writer_t writer;
  lso_writer_initialize(&writer, sizeof(4 + 4 + 32));

  for(int i = 0; i < MAX_ROOMS; ++i)
  {
    struct room_t* room = room_pool.rooms[i];
    if(room == NULL) continue;
    lso_writer_write_int32(&writer, room->id);
    lso_writer_write_int32(&writer, room->clientsCount);
    lso_writer_write_int32(&writer, MAX_CLIENTS_PER_ROOM);
    lso_writer_write_string(&writer, room->channelName);
  }

  message_t* message = message_create_from_writer(kRoomTag, &writer);
  // lso_writer_destroy(&writer);
  return message;
}