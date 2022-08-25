#include "client.h"
#include "room.h"
#include "room_pool.h"
#include "client_pool.h"

#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

int counter_client = 0;

struct client_t* create_client(struct sockaddr_in address, int connfd) 
{ 
  struct client_t *cli = (struct client_t *)malloc(sizeof(struct client_t));
  cli->address = address;
  cli->sockfd = connfd;
  cli->uid = counter_client++;

  cli->chat_with = NULL;
  cli->last_chat_with = NULL;
  cli->room = NULL;

  pthread_mutex_init(&cli->mutex, NULL);
  pthread_create(&cli->thread, NULL, &_client_handler, (void*)cli);
  return cli;
}

void client_lock(struct client_t* client) 
{
  pthread_mutex_lock(&client->mutex); 
}

void client_unlock(struct client_t* client)
{
  pthread_mutex_unlock(&client->mutex);
}

bool client_send_message(struct client_t* client, char* message) 
{
  if(client == NULL) return false;

  if(write(client->sockfd, message, strlen(message)) < 0) 
  {
    perror("ERROR(client_send_message): write to descriptor failed.");
    return false;
  }
  return true;
}

void client_set_room(struct client_t* client, struct room_t* room) 
{
  if(room == client->room) return;

  client->room = room;
  client->chat_with = NULL;
}

void client_set_chat_with(struct client_t* client, struct client_t* with) 
{
  if(with == client->chat_with) return;

  client->last_chat_with = client->chat_with;
  client->chat_with = with;
}

bool client_is_free(struct client_t* client)
{
  return client->chat_with == NULL;
}

void* _client_handler(void* args) 
{
  struct client_t* client = (struct client_t*)args;

  if(client == NULL) 
  {
    perror("ERROR(_client_handler): Client cannot be NULL. Aborting.\n");
    return NULL;
  }

  char msgBuffer[BUFFER_SIZE]; 
  char name[32];
  bool leaveFlag = false;

  // Name
  if (recv(client->sockfd, name, 32, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 32 - 1)
  {
      printf("Didn't enter the name.\n");
      leaveFlag = true;
  }
  else
  {
      strcpy(client->name, name);
      printf("%s has joined the server\n", client->name);
  }

  room_pool_send_all(client);

  while(!leaveFlag)
  {
    int numBytesRead = read(client->sockfd, msgBuffer, BUFFER_SIZE-1);
    msgBuffer[numBytesRead] = '\0';

    if(numBytesRead > 0) 
    {
      if(strlen(msgBuffer) > 0)
      {
        if(client->room == NULL)
        {
          if(strcmp(msgBuffer, "0"))
          {
            int selectedRoomIndex = atoi(msgBuffer);
            struct room_t* room = room_pool_get_by_index(selectedRoomIndex);

            if(room != NULL) 
            {
              room_add_client(room, client);

              client_send_message(client, "Waiting for pairing...\n");
            }
          }
        }

        if(client->room != NULL && strcmp(msgBuffer, "/exitroom\n") == 0) 
        {
          room_remove_client(client->room, client);
        }

        if(client->chat_with != NULL) 
        {
          if(strcmp(msgBuffer, "/exitchat\n") == 0) 
          {
            sprintf(msgBuffer, "%s left the chat. Searching for new match...\n", client->name);
            client_send_message(client->chat_with, msgBuffer);

            client_set_chat_with(client->chat_with, NULL);

            client_send_message(client, "You've left the chat. Searching for new match...\n");
            client_set_chat_with(client, NULL);
          }
          else
          {
            client_send_message(client->chat_with, msgBuffer);
          }
        }
      }
    }
    else if(numBytesRead == 0 || strcmp(msgBuffer, "/exit") == 0)
    {
      leaveFlag = true;
    }

    bzero(msgBuffer, BUFFER_SIZE);
  }


  if(client->room != NULL)
  {
    room_remove_client(client->room, client);
  }

  printf("%s left the server\n", client->name);

  client_pool_remove(client);

  close(client->sockfd);
  pthread_detach(pthread_self());

  free(client);
}