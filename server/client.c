#include "client.h"

#include "room.h"
#include "client_pool.h"
#include "utils.h"
#include "messages.h"
#include "lso_reader.h"
#include "lso_writer.h"
#include "tags.h"
#include "database.h"

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
  cli->isLogged = false;
  cli->room = NULL;

  pthread_mutex_init(&cli->mutex, NULL);
  pthread_create(&cli->thread, NULL, &_client_handler, (void *)cli);
  return cli;
}

void client_lock(client_t *client)
{
  pthread_mutex_lock(&client->mutex);
}

void client_unlock(client_t *client)
{
  pthread_mutex_unlock(&client->mutex);
}

void _handle_request_rooms_tag(client_t *client)
{
  lso_writer_t writer;
  lso_writer_initialize(&writer, sizeof(4 + 4 + 32));

  for (int i = 0; i < MAX_ROOMS; ++i)
  {
    room_t *room = gRooms[i];
    if (room == NULL)
      continue;
    lso_writer_write_int32(&writer, room->id);
    lso_writer_write_int32(&writer, room->clientsCount);
    lso_writer_write_int32(&writer, MAX_CLIENTS_PER_ROOM);
    lso_writer_write_string(&writer, room->channelName);
  }

  message_t *message = message_create_from_writer(kRoomTag, &writer);
  client_send(client, message);

  message_delete(message);
}

void _handle_join_room_tag(client_t *client, lso_reader_t *reader)
{
  int32_t roomId = lso_reader_read_int32(reader);
  room_t *room = gRooms[roomId];

  if (room != NULL)
  {
    if (room_try_join(room, client))
    {
      lso_writer_t writer;
      lso_writer_initialize(&writer, 4);
      lso_writer_write_int32(&writer, roomId);

      message_t *joinAcceptedMessage = message_create_from_writer(kJoinRoomAcceptedTag, &writer);
      client_send(client, joinAcceptedMessage);

      message_delete(joinAcceptedMessage);
    }
    else
    {
      lso_writer_t writer;
      lso_writer_initialize(&writer, 4);
      lso_writer_write_int32(&writer, roomId);

      message_t *joinRoomRefusedMessage = message_create_from_writer(kJoinRoomRefusedTag, &writer);
      client_send(client, joinRoomRefusedMessage);

      message_delete(joinRoomRefusedMessage);
    }
  }
}

// TODO(ciro): TEST THIS METHOD
void _handle_message_tag(client_t* client, lso_reader_t* reader)
{
  room_t* room = client->room;

  if (room == NULL)
  {
    message_t *message = message_create_empty(kRejectSentMessageTag);
    client_send(client, message);

    message_delete(message);

    printf("Rejecting message from client %s", client->name);
    return;
  }

  char *messageText;
  int messageLength = lso_reader_read_string(reader, &messageText);
  printf("Client %s sent message \"%s\".\n", client->name, messageText);

  lso_writer_t writer;
  lso_writer_initialize(&writer, messageLength);
  lso_writer_write_string(&writer, client->name);
  lso_writer_write_string(&writer, messageText);

  message_t* message = message_create_from_writer(kMessageTag, &writer);
  for(int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i) 
  {
    struct client_t* c = room->clients[i];

    if(c == NULL) 
    {
      continue;
    }

    client_send(c, message);
  }

  message_delete(message);
}

void _handle_sign_up_tag(struct client_t* client, lso_reader_t* reader) 
{
  char* name; 
  char* password;

  lso_reader_read_string(reader, &name);
  lso_reader_read_string(reader, &password);  

  if(database_new_user(name, password))
  {
    printf("Account %s created successfully\n", name);

    message_t* message = message_create_empty(kSignUpAcceptedTag);
    client_send(client, message);

    message_delete(message);
  }
  else 
  {
    printf("Account %s not created. Username already in use.\n", name);

    message_t* message = message_create_empty(kSignUpRejectedTag);
    client_send(client, message);

    message_delete(message);
  }
}

void _handle_sign_in_tag(struct client_t* client, lso_reader_t* reader) 
{
  printf("Handling signin message\n");

  char* username;
  char* password;
  lso_reader_read_string(reader, &username);
  lso_reader_read_string(reader, &password);

  printf("Trying to log-in user %s with password %s\n", username, password);

  if(database_user_login(username, password))
  {
    printf("User %s logged successfully\n", username);
    
    client->name = username;
    client->isLogged = true;

    message_t* message = message_create_empty(kSignInAcceptedTag);
    client_send(client, message);

    message_delete(message);
  }
  else
  {
    printf("User %s login failed\n", username);
    
    strcpy(client->name, "");
    client->isLogged = false;

    message_t* message = message_create_empty(kSignInRejectedTag);
    client_send(client, message);

    message_delete(message);
  }
}

void _on_message_received(client_t *client, message_t *message)
{
  lso_reader_t *reader = message_to_reader(message);
  if (message->tag == RequestRoomsTag)
  {
    _handle_request_rooms_tag(client);
  }
  else if (message->tag == kJoinRoomTag)
  {
    _handle_join_room_tag(client, reader);
  }
  else if (message->tag == kMessageTag)
  {
    _handle_message_tag(client, reader);
  }
  else if (message->tag == kLeaveRoomTag)
  {
    room_leave(client->room, client);
  }
  else if(message->tag == kSignUpRequestedTag)
  {
    _handle_sign_up_tag(client, reader);
  }
  else if(message->tag == kSignInRequestedTag)
  {
    _handle_sign_in_tag(client, reader);
  }

  lso_reader_delete(reader);
}

void* _client_handler(void *args)
{
  client_t *client = (client_t *)args;

  if (client == NULL)
  {
    perror("ERROR(_client_handler): Client cannot be NULL. Aborting.\n");
    return NULL;
  }

  printf("New user thread \n");

  int8_t msgBuffer[BUFFER_SIZE];
  bool leaveFlag = false;

  while (!leaveFlag)
  {
    int32_t size = read(client->sockfd, msgBuffer, BUFFER_SIZE - 1);
    if (size == 0)
    {
      leaveFlag = true;
      continue;
    }

    byte_buffer_t *byteBuffer = byte_buffer_create_from_bytes(size, msgBuffer);
    message_t *message = message_create_from_byte_buffer(byteBuffer);

    printf("Message with tag %d received: ByteBuffer(Count: %d, Capacity: %d)\n", message->tag, byteBuffer->count, byteBuffer->capacity);

    _on_message_received(client, message);

    message_delete(message);
    // byte_buffer_delete(byteBuffer);

    bzero(msgBuffer, BUFFER_SIZE);
  }

  room_leave(client->room, client);

  printf("%s left the server\n", client->name);

  client_pool_remove(client);

  pthread_detach(pthread_self());

  close(client->sockfd);
  free(client);
  return NULL;
}

bool client_send(client_t *client, message_t *message)
{
  // byte_buffer_print_debug(message->buffer);

  byte_buffer_t *buffer = message_to_buffer(message);

  // byte_buffer_print_debug(buffer);

  printf("Sending message with tag %d to %s\n", message->tag, client->name);
  if (send(client->sockfd, buffer->buffer, buffer->count, 0) < 0)
  {
    printf("DEBUG: Error sending message with tag: %d\n", message->tag);
    return false;
  }
  byte_buffer_delete(buffer);
  return true;
}