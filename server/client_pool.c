#include "client_pool.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

struct client_pool_t pool;

int get_client_pool_empty_index()
{
  pthread_mutex_lock(&pool.mutex);

  int index = -1;
  for(int i = 0; i < MAX_CLIENTS; ++i) 
  {
    if(pool.clients[i] != NULL) continue;
    index = i;
    break;
  }

  pthread_mutex_unlock(&pool.mutex);

  return index;
}

int find_index(struct client_t* client)
{
  int index = -1;
  if(client != NULL) 
  {
    pthread_mutex_lock(&pool.mutex);

    for(int i = 0; i < MAX_CLIENTS; ++i) 
    {
      struct client_t* temp = pool.clients[i];
      if(temp == NULL || temp->uid != client->uid) continue;
      index = i;
      break;
    }

    pthread_mutex_unlock(&pool.mutex);
  }

  return index;
}

bool client_pool_add(struct client_t* client)
{
  if(client == NULL)
  {
    return false;
  }

  int index = get_client_pool_empty_index();

  if(index == -1) 
  {
    perror("ERROR(client_pool_add): unable to add new client. Aborting.\n");
    return false;
  }

  pthread_mutex_lock(&pool.mutex);

  pool.clients[index] = client;
  pool.size++;

  pthread_mutex_unlock(&pool.mutex);

  return true;
}

bool client_pool_remove(struct client_t* client)
{
  if(client == NULL)
  {
    return false;
  }

  int index = find_index(client);

  if(index == -1)
  {
    perror("ERROR(client_pool_remove): unable to find client. Aborting.");
    return false;
  }

  
  pthread_mutex_lock(&pool.mutex);

  pool.clients[index] = NULL;
  pool.size--;

  pthread_mutex_unlock(&pool.mutex);

  return true;
}

bool client_pool_is_full()
{
  return pool.size == MAX_CLIENTS;
}