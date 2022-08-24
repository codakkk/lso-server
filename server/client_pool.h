
#ifndef __H_CLIENT_POOL__
#define __H_CLIENT_POOL__

#include "client.h"
#include <pthread.h>
#include <stdbool.h>
#define MAX_CLIENTS 10

struct client_pool_t {
  struct client_t *clients[MAX_CLIENTS];
  int size;

  pthread_mutex_t mutex;
};

bool client_pool_add(struct client_t* cl);
bool client_pool_remove(struct client_t* cl);
bool client_pool_is_full();
#endif