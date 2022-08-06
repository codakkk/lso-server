#include "room.h"

room_t* create_room(char name[32])
{
  room_t* room = (room_t*)malloc(sizeof(room_t));
  room->id = counter_room++;
  
  pthread_create(&room->tid, NULL, &handle_room, (void*)room);

  strcpy(room->channelName, name);
  return room;
}

/* Add clients to room */
void add_client_room(room_t *room, client_t *cl)
{
    pthread_mutex_lock(&room_mutex);

    for(int i=0; i < MAX_CLIENTS_PER_ROOM; ++i)
    {
        if(!room->clients[i])
        {
            room->clients[i] = cl;
            break;
        }
    }

    pthread_mutex_unlock(&room_mutex);
}

/* Remove clients to room */
void remove_client_room(room_t *room, int uid)
{
    pthread_mutex_lock(&room_mutex);

    for(int i=0; i < MAX_CLIENTS_PER_ROOM; ++i)
    {
        if(room->clients[i])
        {
            if(room->clients[i]->uid == uid)
            {
                room->clients[i] = NULL;
                break;
            }
        }
    }

    pthread_mutex_unlock(&room_mutex);
}

/* Count client in room */
int count_in_room(room_t* room)
{
  pthread_mutex_lock(&room_mutex);
    int counter_in_room = 0;
    for(int i=0; i < MAX_CLIENTS_PER_ROOM; ++i)
    {
        if(room->clients[i])
        {
          counter_in_room++;
        }
    }

  pthread_mutex_unlock(&room_mutex);
  return counter_in_room;
}

void* handle_room(void* arg) 
{
  client_t* cli = (client_t*)arg;

}