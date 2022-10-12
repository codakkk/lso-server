#include "room.h"
#include "client.h"
#include "room_pool.h"
#include "tags.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_SZ 2048

int counter_room = 0;

int find_index(struct room_t* room)
{
  int index = -1;
  for (int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
  {
    if (!room->clients[i])
    {
      index = i;
      break;
    }
  }
  return index;
}


int find_client_index(struct room_t* room, struct client_t* client)
{
  int index = -1;
  for (int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
  {
    struct client_t* roomClient = room->clients[i];
    if (roomClient == NULL || roomClient->uid != client->uid) continue;
    index = i;
    break;
  }
  return index;
}

struct room_t *room_create(char name[32])
{
  struct room_t *room = (struct room_t *)malloc(sizeof(struct room_t));
  room->id = counter_room++;

  pthread_mutex_init(&room->mutex, NULL);
  pthread_create(&room->tid, NULL, &room_update, (void *)room);

  strcpy(room->channelName, name);
  return room;
}

void room_lock(struct room_t* room) 
{
  pthread_mutex_lock(&room->mutex);
}

void room_unlock(struct room_t* room)
{
  pthread_mutex_unlock(&room->mutex);
}

/* Add clients to room */
void room_add_client(struct room_t *room, struct client_t *client)
{
  room_lock(room);

  int index = find_index(room);
  if(index != -1)
  {
    room->clients[index] = client;
    room->clientsCount++;

    client_set_room(client, room);

    _on_enter_room(room, client);
  }

  room_unlock(room);
}

/* Remove client from room */
void room_remove_client(struct room_t* room, struct client_t* client)
{
  room_lock(room);
  
  int index = find_client_index(room, client);
  if(index != -1)
  {
    room->clients[index] = NULL;
    room->clientsCount--;

    _on_leave_room(room, client);

    client_set_room(client, NULL);
  }

  room_unlock(room);
}

/* Count client in room */
int room_count_clients(struct room_t *room)
{
  return room->clientsCount;
}

void *room_update(void *arg)
{
  struct room_t* room = (struct room_t*)arg;

  while (1)
  {
    _room_try_matches(room);

    sleep(1);
  }

  free(room);
  // Room should die here
}

void _room_try_matches(struct room_t* room)
{
  struct client_t* first = NULL;
  for(int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i) 
  {
    struct client_t* temp = room->clients[i];
    if(temp == NULL || !client_is_free(temp)) continue;
    

    // If we already found a match, check if the uid matches.
    // If so, just skip, because we cannot match with ourself
    if(first != NULL)
    {
      if(first->uid == temp->uid) continue;
      if(first->last_chat_with != NULL && first->last_chat_with->uid == temp->uid) continue;
    }

    if(first == NULL) 
    {
      first = temp;
    }
    else 
    {
      client_lock(first);
      client_lock(temp);

      client_set_chat_with(first, temp);
      client_set_chat_with(temp, first);
      
      client_unlock(first);
      client_unlock(temp);

      _on_match(room, first, temp);
      break;
    }
  }
}

void _on_match(struct room_t* room, struct client_t* c1, struct client_t* c2)
{
  // Send message to c1
  lso_writer_t writer;
  lso_writer_initialize(&writer, 8);
  lso_writer_write_int32(&writer, room->id);
  lso_writer_write_int32(&writer, c2->uid);
  lso_writer_write_string(&writer, c2->name);

  message_t* c1MatchedMessage = message_create_from_writer(kOnMatchTag, &writer);
  client_send(c1, c1MatchedMessage);

  message_destroy(c1MatchedMessage);

  // Send message to c2
  lso_writer_t c2Writer;
  lso_writer_initialize(&c2Writer, 8);
  lso_writer_write_int32(&c2Writer, room->id);
  lso_writer_write_int32(&c2Writer, c1->uid);
  lso_writer_write_string(&c2Writer, c1->name);

  message_t* c2MatchedMessage = message_create_from_writer(kOnMatchTag, &c2Writer);
  client_send(c2, c2MatchedMessage);
  
  message_destroy(c2MatchedMessage);

  printf("[%s] Matched %s with %s\n", room->channelName, c1->name, c2->name);
}

void _on_leave_room(struct room_t* room, struct client_t* client)
{
  // room_pool_send_all(client);

  if(client->chat_with != NULL)
  {
    char buff_out[BUFFER_SZ];

    client_set_chat_with(client->chat_with, NULL);

    sprintf(buff_out, "%s left the chat. Going back to room.\nWaiting for pairing...\n", client->name);
    client_send_message(client->chat_with, buff_out);
  }

  printf("%s has left the room %s\n", client->name, room->channelName);
}

void _on_enter_room(struct room_t* room, struct client_t* client)
{
  printf("%s has joined the room %s\n", client->name, room->channelName);
}