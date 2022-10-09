

#ifndef __H_ROOM_POOL__
#define __H_ROOM_POOL__

#include "room.h"
#include <pthread.h>
#include <stdbool.h>

#define MAX_ROOMS 10

struct room_pool_t {
  struct room_t* rooms[MAX_ROOMS];

  int size;
};

void room_pool_initialize();
bool room_pool_add(struct room_t* room);
void room_pool_list_all();

struct room_t* room_pool_get_by_id(int32_t id);

message_t* create_rooms_message();

struct room_t* room_pool_get_by_index(int index);
#endif