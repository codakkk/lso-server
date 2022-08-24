#include "room.h"
#include "client.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_SZ 2048

int counter_room = 0;

struct room_t *room_create(char name[32])
{
  struct room_t *room = (struct room_t *)malloc(sizeof(struct room_t));
  room->id = counter_room++;

  pthread_mutex_init(&room->mutex, NULL);
  pthread_create(&room->tid, NULL, &room_update, (void *)room);

  strcpy(room->channelName, name);
  return room;
}

/* Add clients to room */
void room_add_client(struct room_t *room, struct client_t *client)
{
  pthread_mutex_lock(&room->mutex);

  for (int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
  {
    if (!room->clients[i])
    {
      room->clients[i] = client;

      client_enter_room(client, room);
      
      printf("%s has joined the room %s\n", client->name, room->channelName);
      break;
    }
  }

  pthread_mutex_unlock(&room->mutex);
}

/* Remove client from room */
void room_remove_client(struct room_t* room, struct client_t* client)
{
  pthread_mutex_lock(&room->mutex);

  for (int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
  {
    struct client_t* roomClient = room->clients[i];
    if (roomClient == NULL || roomClient->uid != client->uid) continue;

    client_leave_room(client);
    room->clients[i] = NULL;

    printf("%s has left the room %s\n", client->name, room->channelName);
    break;
  }

  pthread_mutex_unlock(&room->mutex);
}

/* Count client in room */
int room_count_clients(struct room_t *room)
{
  pthread_mutex_lock(&room->mutex);

  int counter_in_room = 0;
  for (int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
  {
    struct client_t *client = room->clients[i];
    if (client != NULL)
    {
      counter_in_room++;
    }
  }

  pthread_mutex_unlock(&room->mutex);
  return counter_in_room;
}

void *room_update(void *arg)
{
  struct room_t* room = (struct room_t*)arg;

  while (1)
  {
    pthread_mutex_lock(&room->mutex);

    struct client_t* temp = NULL;
    struct client_t* first = NULL;

    for(int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i) 
    {
      temp = room->clients[i];
      if(temp != NULL && client_is_free(temp))
      {
        first = room->clients[i];
        break;
      }
    }

    if(first != NULL) 
    {
      for(int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i) 
      {
        temp = room->clients[i];
        if(temp != NULL && temp->uid != first->uid && client_is_free(temp))
        {
          first->chat_with = temp;
          temp->chat_with = first;
          break;
        }
      }
    }

    
    pthread_mutex_unlock(&room->mutex);

    sleep(1);
  }

    // Room should die here
}

void _room_start_chat(struct client_t *cli)
{
  char buff_out[BUFFER_SZ];
  int leave_flag = 0;

  sprintf(buff_out, "Matched with %s\n", cli->name);
  client_send_message(cli, buff_out);
  bzero(buff_out, BUFFER_SZ);

  while (cli->room != NULL)
  {
    if (leave_flag)
    {
      bzero(buff_out, BUFFER_SZ);
      client_leave_chat(cli);
      cli->chat_with = NULL;
      break;
    }

    int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
    if (receive > 0)
    {
      if (strlen(buff_out) > 0)
      {
        if (strcmp(buff_out, "exit_room\n\0") == 0 || strcmp(buff_out, "exit_chat\n\0") == 0 || strcmp(buff_out, "exit_chat_AKW\0") == 0)
        {
          if (strcmp(buff_out, "exit_chat\n\0") == 0 || strcmp(buff_out, "exit_chat_AKW\0") == 0)
          {
            printf("%s exit form chat\n", cli->name);
            leave_flag = 1;
            if (strcmp(buff_out, "exit_chat\n\0") == 0)
            {
              client_send_message(cli->chat_with, buff_out);
            }

            client_leave_chat(cli);
          }
          else if (strcmp(buff_out, "exit_room\n\0") == 0)
          {
            printf("%s exit form room", cli->name);

            leave_flag = 1;
            client_leave_room(cli);
          }
        }
        else
        {
          client_send_message(cli->chat_with, buff_out);
        }
      }
    }
    else if (receive == 0 || strcmp(buff_out, "exit") == 0)
    {
      printf("%s exit\n", cli->name);
      sprintf(buff_out, "exit");
      leave_flag = 1;

      client_leave_room(cli);
      client_send_message(cli->chat_with, buff_out);
    }
    else
    {
      printf("ERROR: -1\n");
      leave_flag = 1;
    }

    bzero(buff_out, BUFFER_SZ);
  }
}