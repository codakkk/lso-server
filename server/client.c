#include "client.h"
#include "room.h"
#include "room_pool.h"
#include "client_pool.h"
#include "utils.h"
#include "messages.h"
#include "lso_reader.h"
#include "lso_writer.h"
#include "tags.h"

#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 128

int counter_client = 0;

struct client_t* client_create(struct sockaddr_in address, int connfd) 
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

void _on_message_received(struct client_t* client, message_t* message)
{
  lso_reader_t* reader = message_to_reader(message);
  if(message->tag == SendFirstConfigurationTag) 
  {
    lso_reader_read_string(reader, &client->name);
    printf("%s has joined the server\n", client->name);

    message_t* message = message_create_empty(FirstConfigurationAcceptedTag);
    client_send(client, message);
    
    message_destroy(message);
    free(message);
  }
  else if(message->tag == RequestRoomsTag)
  {
    message_t* message = create_rooms_message();

    client_send(client, message);
  }
  else if(message->tag == kJoinRoomTag)
  {
    int32_t roomId = lso_reader_read_int32(reader);
    struct room_t* room = room_pool_get_by_id(roomId);

    if(room != NULL) 
    {
      if(room->clientsCount < MAX_CLIENTS_PER_ROOM) 
      {
        room_add_client(room, client);

        lso_writer_t writer;
        lso_writer_initialize(&writer, 4);
        lso_writer_write_int32(&writer, roomId);

        message_t* joinAcceptedMessage = message_create_from_writer(kJoinRoomAcceptedTag, &writer);
        client_send(client, joinAcceptedMessage);

        message_destroy(joinAcceptedMessage);
        free(joinAcceptedMessage);
      }
      else
      {
        lso_writer_t writer;
        lso_writer_initialize(&writer, 4);
        lso_writer_write_int32(&writer, roomId);

        message_t* joinRoomRefusedMessage = message_create_from_writer(kJoinRoomRefusedTag, &writer);
        client_send(client, joinRoomRefusedMessage);

        message_destroy(joinRoomRefusedMessage);
        free(joinRoomRefusedMessage);
      }
    }
  }
  else if(message->tag == kSendMessageTag)
  {
    struct client_t* other = client->chat_with;
    if(other != NULL) 
    {
      char* messageText;
      int messageLength = lso_reader_read_string(reader, &messageText);
      printf("%s sending message %s to %s.", client->name, messageText, other->name);

      lso_writer_t writer;
      lso_writer_initialize(&writer, messageLength);
      lso_writer_write_string(&writer, messageText);

      message_t* message = message_create_from_writer(kConfirmSentMessageTag, &writer);
      client_send(client, message);

      free(message);
      
      message = message_create_from_writer(kSendMessageTag, &writer);
      client_send(other, message);

      message_destroy(message);
      free(message);
    } 
    else 
    {
      message_t* message = message_create_empty(kRejectSentMessageTag);
      client_send(client, message);
      
      message_destroy(message);
      free(message);

      printf("Rejecting message from client %s", client->name);
    }
  }

  free(reader);
}

void* _client_handler(void* args) 
{
  struct client_t* client = (struct client_t*)args;

  if(client == NULL) 
  {
    perror("ERROR(_client_handler): Client cannot be NULL. Aborting.\n");
    return NULL;
  }

  // Send a message to the client
  // saying that he can now send messages

  message_t* acceptedMessage = message_create_empty(JoinRequestAcceptedTag);
  client_send(client, acceptedMessage);
  free(acceptedMessage);

  char msgBuffer[BUFFER_SIZE]; 
  bool leaveFlag = false;

  while(!leaveFlag)
  {
    int32_t size = read(client->sockfd, msgBuffer, BUFFER_SIZE-1);

    if(size == 0) 
    {
      leaveFlag = true;
      continue;
    }

    byte_buffer_t* byteBuffer = byte_buffer_create_from_bytes(size, msgBuffer);
    message_t* message = message_create_from_byte_buffer(byteBuffer);

    printf("Message with tag %d received: ByteBuffer(Count: %d, Capacity: %d)\n", message->tag, byteBuffer->count, byteBuffer->capacity);

    _on_message_received(client, message);

    //message_destroy(message);
    free(message);
    free(byteBuffer);

    /*
    if(numBytesRead > 0) 
    {
      if(strlen(msgBuffer) > 0)
      {
        if(strcmp(msgBuffer, "/exit\n") == 0)
        {
          leaveFlag = true;
        }
        else if(client->room == NULL)
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

        else if(client->room != NULL && strcmp(msgBuffer, "/exitroom\n") == 0) 
        {
          room_remove_client(client->room, client);
        }

        else if(client->chat_with != NULL) 
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
    else if(numBytesRead == 0)
    {
      leaveFlag = true;
    }
    */
    bzero(msgBuffer, BUFFER_SIZE);
  }


  if(client->room != NULL)
  {
    room_remove_client(client->room, client);
  }

  printf("%s left the server\n", client->name);

  client_pool_remove(client);

  pthread_detach(pthread_self());

  close(client->sockfd);
  free(client);
}

bool client_send(struct client_t* client, message_t* message)
{
  // byte_buffer_print_debug(message->buffer);
  
  byte_buffer_t* buffer = message_to_buffer(message);
  
  // byte_buffer_print_debug(buffer);

  printf("Sending message with tag %d to %s\n", message->tag, client->name);
  if(send(client->sockfd, buffer->buffer, buffer->count, 0) < 0) 
  {
    printf("DEBUG: Error sending message with tag: %d\n", message->tag);
    return false;
  }
  byte_buffer_destroy(buffer);
  free(buffer);
  return true;
}