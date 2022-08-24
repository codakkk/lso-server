

#ifndef __H_ROOM_POOL__
#define __H_ROOM_POOL__

#include "room.h"
#include <pthread.h>
#include <stdbool.h>

#define MAX_ROOMS 10

struct room_pool_t {
  struct room_t* rooms[MAX_ROOMS];
  pthread_mutex_t mutex;

  int size;
};

bool room_pool_add(struct room_t* room);
void room_pool_send_all(struct client_t* client);

struct room_t* room_pool_get_by_index(int index);
#endif