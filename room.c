#include "room.h"

room_t* create_room(client_t* client1, client_t* client2, char name[32])
{
  room_t* room = (room_t*)malloc(sizeof(room_t));
  room->id = counter++;
  
  pthread_create(&room->tid, NULL, &handle_room, (void*)room);

  strcpy(room->channelName, name);
  return room;
}

int add_client(room_t* room, client_t* client) 
{
  if(room == NULL || client == NULL) return 0;

  for(int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i) 
  {
    client_t* roomClient = room->clients[i];
    
    if(roomClient == NULL) continue;

    if(roomClient->uid == client->uid) return 0;
  }

  int index = find_empty_index(room);

  if(index == -1) return 0;

  room->clients[index] = client;
  return 1;
}

int find_empty_index(room_t* room)
{
  for(int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i) 
  {
    if(room->clients[i] == NULL) return i;
  }
  return -1;
}

void* handle_room(void* arg) 
{
  client_t* cli = (client_t*)arg;

}