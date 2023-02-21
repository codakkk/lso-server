
#ifndef __H_CLIENT_POOL__
#define __H_CLIENT_POOL__

#include "client.h"
#include <pthread.h>
#include <stdbool.h>
#define MAX_CLIENTS 10

struct client_pool_t {
  client_t *clients[MAX_CLIENTS];
  int size;

  pthread_mutex_t mutex;
};

bool client_pool_add(client_t* cl);
bool client_pool_remove(client_t* cl);
bool client_pool_is_full();

void client_pool_lock();
void client_pool_unlock();
#endif