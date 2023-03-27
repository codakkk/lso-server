#include "client_pool.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

struct client_pool_t client_pool;

int get_client_pool_empty_index()
{
  int index = -1;
  for(int i = 0; i < MAX_CLIENTS; ++i) 
  {
    if(client_pool.clients[i] != NULL) continue;
    index = i;
    break;
  }

  return index;
}

int find_index_in_client_pool(client_t* client)
{
  int index = -1;
  if(client != NULL) 
  {
    for(int i = 0; i < MAX_CLIENTS; ++i) 
    {
      client_t* temp = client_pool.clients[i];
      if(temp == NULL || temp->uid != client->uid) continue;
      index = i;
      break;
    }
  }

  return index;
}

client_t* client_pool_get(int32_t id)
{
	client_t* client = NULL;

	for(uint32_t i = 0; i < client_pool.size; ++i)
	{
		client_t* temp = client_pool.clients[i];
		if(temp == NULL || temp->user.name == NULL ||  temp->user.id != id)
		{
			continue;
		}

		client = temp;
		break;
	}

  return client;
}

bool client_pool_add(client_t* client)
{
  if(client == NULL) return false;

  int index = get_client_pool_empty_index();

  if(index == -1) 
  {
    perror("ERROR(client_pool_add): unable to add new client. Aborting.\n");
    return false;
  }
  
  client_pool.clients[index] = client;
  client_pool.size++;

  return true;
}

bool client_pool_remove(client_t* client)
{
  if(client == NULL)
  {
    return false;
  }

  int index = find_index_in_client_pool(client);

  if(index == -1)
  {
    perror("ERROR(client_pool_remove): unable to find client. Aborting.");
    return false;
  }
  
  client_pool.clients[index] = NULL;
  client_pool.size--;

  return true;
}

bool client_pool_is_full()
{
  return client_pool.size == MAX_CLIENTS;
}

void client_pool_lock()
{
  pthread_mutex_lock(&client_pool.mutex);
}
void client_pool_unlock()
{
  pthread_mutex_unlock(&client_pool.mutex);
}