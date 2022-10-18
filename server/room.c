#include "room.h"
#include "client.h"
#include "tags.h"
#include "chat.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int counter_room = 0;
room_t* gRooms[10];

int32_t find_empty_chat_index(room_t* room)
{
  int index = -1;
  for (int i = 0; i < MAX_CHAT_PER_ROOM; ++i)
  {
    if(room->chats[i] != NULL) continue;
    index = i;
    break;
  }
  return index;
}

int32_t find_chat_by_index(room_t* room, chat_t* chat)
{
  int index = -1;
  for (int i = 0; i < MAX_CHAT_PER_ROOM; ++i)
  {
    chat_t* tempChat = room->chats[i];
    if (tempChat == NULL || tempChat != chat) continue;
    index = i;
    break;
  }
  return index;
}

int find_index(room_t* room)
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

int find_client_index(room_t* room, client_t* client)
{
  int index = -1;
  for (int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
  {
    client_t* roomClient = room->clients[i];
    if (roomClient == NULL || roomClient->uid != client->uid) continue;
    index = i;
    break;
  }
  return index;
}

room_t *room_create(char name[32])
{
  room_t *room = (room_t *)malloc(sizeof(room_t));
  room->id = counter_room++;

  pthread_mutex_init(&room->mutex, NULL);
  pthread_create(&room->tid, NULL, &room_update, (void *)room);

  strcpy(room->channelName, name);
  return room;
}

void room_delete(room_t* room)
{
  if(room == NULL) {
    return;
  }

  for(int i = 0; i < MAX_CHAT_PER_ROOM; ++i) 
  {
    if(room->chats[i] == NULL) continue;
    chat_delete(room->chats[i]);

    room->chats[i] = NULL;
  }

  free(room);
}

void room_lock(room_t* room) 
{
  pthread_mutex_lock(&room->mutex);
}

void room_unlock(room_t* room)
{
  pthread_mutex_unlock(&room->mutex);
}

/* Add clients to room */
bool room_try_join(room_t *room, client_t *client)
{
  int index = find_index(room);

  // Room is full
  if(index == -1) 
  {
    return false;
  }

  room_lock(room);
  
  room->clients[index] = client;
  room->clientsCount++;

  client->room = room;
  client->current_chat = NULL;

  room_unlock(room);

  _on_enter_room(room, client);
}

/* Remove client from room */
bool room_leave(room_t* room, client_t* client)
{ 
  if(room == NULL) 
  {
    return false;
  }

  int index = find_client_index(room, client);
  if(index == -1)
  {
    return false;
  }

  room_lock(room);
  
  room->clients[index] = NULL;
  room->clientsCount--;

  room_unlock(room);

  client->room = NULL;
  _on_leave_room(room, client);
}

void _room_try_matches(room_t* room)
{
  client_t* first = NULL;
  for(int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i) 
  {
    client_t* temp = room->clients[i];
    if(temp == NULL || temp->current_chat != NULL) continue;
    

    // If we already found a match, check if the uid matches.
    // If so, just skip, because we cannot match with ourself
    if(first != NULL)
    {
      if(first->uid == temp->uid) continue;
      if(first->last_match != NULL && first->last_match->uid == temp->uid) continue;
    }

    if(first == NULL) 
    {
      first = temp;
    }
    else 
    {
      _on_match(room, first, temp);
      break;
    }
  }
}

void _on_match(room_t* room, client_t* c1, client_t* c2)
{
  int32_t chatIndex = find_empty_chat_index(room);
  if(chatIndex != -1)
  {
    chat_t* chat = chat_create(c1, c2);

    client_lock(c1);
    client_lock(c2);
    
    c1->current_chat = c2->current_chat = chat;
    
    client_unlock(c1);
    client_unlock(c2);

    room->chats[chatIndex] = chat;
    
    printf("[%s] Matched %s with %s\n", room->channelName, c1->name, c2->name);
  }
}

void _on_leave_room(room_t* room, client_t* client)
{
  chat_t* clientChat = client->current_chat;

  if(clientChat != NULL)
  {
    int32_t chatIndex = find_chat_by_index(room, clientChat);

    client_t* clientMatch = clientChat->client1->uid == client->uid ? clientChat->client2 : clientChat->client1;

    chat_close(clientChat);

    message_t* message = message_create_empty(kLeaveChatTag);
    client_send(clientMatch, message);

    printf("Client %s left chat with %s.\n", client->name, clientMatch->name);

    chat_delete(client->current_chat);
  }

  printf("%s left room %s.\n", client->name, room->channelName);
}

void _on_enter_room(room_t* room, client_t* client)
{
  printf("%s has joined the room %s\n", client->name, room->channelName);
}

void *room_update(void *arg)
{
  room_t* room = (room_t*)arg;

  while (1)
  {
    _room_try_matches(room);

    for(int i = 0; i < MAX_CHAT_PER_ROOM; ++i)
    {
      chat_t* chat = room->chats[i];
      if(chat == NULL) continue;

      if(chat_is_over(chat))
      {
        chat_delete(chat);
        room->chats[i] = NULL;

        message_t* message = message_create_empty(kChatTimeoutTag);

        client_send(chat->client1, message);
        client_send(chat->client2, message);
        
        message_delete(message);
      }
    }

    sleep(1);
  }

  room_delete(room);
  // Room should die here
}
