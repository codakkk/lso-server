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

client_t* client_create(struct sockaddr_in address, int connfd) 
{ 
  client_t *cli = (client_t *)malloc(sizeof(client_t));
  cli->address = address;
  cli->sockfd = connfd;
  cli->uid = counter_client++;

  cli->match = NULL;
  cli->last_match = NULL;
  cli->room = NULL;

  pthread_mutex_init(&cli->mutex, NULL);
  pthread_create(&cli->thread, NULL, &_client_handler, (void*)cli);
  return cli;
}

void client_lock(client_t* client) 
{
  pthread_mutex_lock(&client->mutex); 
}

void client_unlock(client_t* client)
{
  pthread_mutex_unlock(&client->mutex);
}

void _on_message_received(client_t* client, message_t* message)
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
    room_t* room = room_pool_get_by_id(roomId);

    if(room != NULL) 
    {
      if(room->clientsCount < MAX_CLIENTS_PER_ROOM) 
      {
        if(client->room != NULL)  
        {
          room_remove_client(client->room, client);
          client->room = NULL;
        }

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
    client_t* other = client->match;
    if(other != NULL) 
    {
      char* messageText;
      int messageLength = lso_reader_read_string(reader, &messageText);
      printf("%s sending message %s to %s.\n", client->name, messageText, other->name);

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
  else if(message->tag == kLeaveRoomTag) 
  {
    if(client->room != NULL)
    {
      client_t* other = client->match;
      if(other != NULL)
      {
        message_t* message = message_create_empty(kLeaveChatTag);
        client_send(other, message);
      }

      room_remove_client(client->room, client);
    }
  }
  else if(message->tag == kLeaveChatTag) 
  {
    client_t* other = client->match;
    if(other != NULL)
    {
      other->last_match = other->match;
      other->match = NULL;

      client->last_match = client->match;
      client->match = NULL;

      message_t* message = message_create_empty(kLeaveChatTag);
      client_send(other, message);
    }
  }
  
  free(reader);
}

void* _client_handler(void* args) 
{
  client_t* client = (client_t*)args;

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

bool client_send(client_t* client, message_t* message)
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