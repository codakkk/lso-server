#include "client.h"

#include "room.h"
#include "client_pool.h"
#include "utils.h"
#include "message.h"
#include "lso_reader.h"
#include "lso_writer.h"
#include "tags.h"
#include "database.h"
#include "messages.h"

#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

int counter_client = 0;



void handle_request_rooms_tag(client_t *client)
{
	message_t* message = create_request_rooms_accepted_message();
  client_send(client, message);

  message_delete(message);
}

void handle_join_room_requested_tag(client_t *client, lso_reader_t *reader)
{
  int32_t roomId = lso_reader_read_int32(reader);
  room_t* room = room_get(roomId);

  if(room == NULL || room_is_full(room))
  {
		message_t* joinRoomRefusedMessage = create_join_room_refused_message(room);
    client_send(client, joinRoomRefusedMessage);

    message_delete(joinRoomRefusedMessage);
    return;
  }

	// First send a request to Room owner and asks him if client can join
	message_t* message = create_join_room_requested_message(client);
	client_send(room->owner, message);

	message_delete(message);
}

void handle_join_room_accepted_tag(client_t *client, lso_reader_t *reader)
{
  int32_t clientId = lso_reader_read_int32(reader);
  int32_t roomId = lso_reader_read_int32(reader);

  client_t* requestingClient = client_pool_get(clientId);
  room_t* room = room_get(roomId);

	room_add_client(room, requestingClient);

	message_t* notifyMessage = create_join_room_notify_accepted_message(requestingClient);
	room_send_message(room, notifyMessage);
	message_delete(notifyMessage);
}

void handle_join_room_refused_tag(client_t* client, lso_reader_t *reader)
{
  int32_t clientId = lso_reader_read_int32(reader);
  client_t* otherClient = client_pool_get(clientId);

	message_t* refusedMessage = create_join_room_refused_message(client->room);
	client_send(otherClient, refusedMessage);
	message_delete(refusedMessage);
}


// TODO(ciro): TEST THIS METHOD
void handle_message_received_tag(client_t* client, lso_reader_t* reader)
{
  room_t* room = client->room;

  if (room == NULL)
  {
    return;
  }

  int8_t* messageText;
  int32_t messageLength = lso_reader_read_string(reader, &messageText);

	message_t* message = create_send_message_message(client, messageText, messageLength);

	room_send_message(room, message);

  message_delete(message);
}

void handle_sign_up_tag(struct client_t* client, lso_reader_t* reader)
{
  printf("handle_sign_up_tag\n");
  int8_t* name;
  int8_t* password;

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

void handle_sign_in_tag(struct client_t* client, lso_reader_t* reader)
{
  int8_t* username;
  int8_t* password;
  lso_reader_read_string(reader, &username);
  lso_reader_read_string(reader, &password);

  bool result = database_user_login(client, username, password);
  if(result)
  {
    printf("[Log-in]: User %s logged successfully\n", username);

    lso_writer_t writer;
    lso_writer_initialize(&writer, 4);
    client_serialize(client, &writer);

    message_t* message = message_create_from_writer(kSignInAcceptedTag, &writer);
    client_send(client, message);

    message_delete(message);
  }
  else
  {
    printf("Client id %d login failed with username: %s\n", client->uid, username);

    message_t* message = message_create_empty(kSignInRejectedTag);
    client_send(client, message);

    message_delete(message);
  }
}

void handle_create_room_tag(struct client_t* client, lso_reader_t* reader)
{
  int8_t* roomName = NULL;
  lso_reader_read_string(reader, &roomName);

  room_t* room = room_create(roomName);

  if(room != NULL)
  {
    printf("Client id %d created room named %s.\n", client->uid, roomName);

    room_add_client(room, client);
		room->owner = client;

		message_t* message = create_room_create_accepted_message(room);
		client_send(client, message);

		message_delete(message);
  }
}

void handle_leave_room_tag(client_t* client, message_t* message)
{
	room_t* room = client->room;
  if(room != NULL)
  {
		if(room->owner->user.id == client->user.id)
		{
			// Close room and kick everyone

			printf("[Room %d]: Closing...\n", room->id);

			message_t* roomClosedMessage = message_create_empty(kRoomClosedTag);
			for(int i = 0; i < MAX_CLIENTS_PER_ROOM; ++i)
			{
				if(room->clients[i] == NULL)
				{
					continue;
				}

				room->clients[i]->room = NULL;
				client_send(room->clients[i], roomClosedMessage);
			}

			message_delete(roomClosedMessage);


			printf("[Room %d]: Closed!\n", room->id);

			room_delete(room);
		}
		else
		{
			room_remove_client(client->room, client);

			message_t* leaveRoomMessage = create_leave_room_message(client);
			room_send_message(room, leaveRoomMessage);
			message_delete(leaveRoomMessage);

			printf("[Room %d]: Client id %d left room.\n", room->id, client->uid);
		}

  }
}

void on_message_received(client_t* client, message_t* message)
{
  lso_reader_t* reader = message_to_reader(message);

  if(message->tag == kRoomCreateRequestedTag)
  {
    handle_create_room_tag(client, reader);
  }
  else if (message->tag == kRequestRoomsTag)
  {
    handle_request_rooms_tag(client);
  }
  else if (message->tag == kJoinRoomRequestTag)
  {
    handle_join_room_requested_tag(client, reader);
  }
  else if(message->tag == kJoinRoomAcceptedTag)
  {
    handle_join_room_accepted_tag(client, reader);
  }
  else if(message->tag == kJoinRoomRefusedTag)
  {
    handle_join_room_refused_tag(client, reader);
  }
  else if (message->tag == kSendMessageTag)
  {
    handle_message_received_tag(client, reader);
  }
  else if (message->tag == kLeaveRoomRequestedTag)
  {
    handle_leave_room_tag(client, message);
  }
  else if(message->tag == kSignUpRequestedTag)
  {
    handle_sign_up_tag(client, reader);
  }
  else if(message->tag == kSignInRequestedTag)
  {
    handle_sign_in_tag(client, reader);
  }

  lso_reader_delete(reader);
}

void* client_handler(void *args)
{
  client_t *client = (client_t *)args;

  if (client == NULL)
  {
    perror("ERROR(client_handler): Client cannot be NULL. Aborting.\n");
    return NULL;
  }

  int8_t msgBuffer[BUFFER_SIZE];
  bool leaveFlag = false;

  while (!leaveFlag)
  {
    int32_t size = read(client->sockfd, msgBuffer, BUFFER_SIZE - 1);
    if (size <= 0)
    {
      leaveFlag = true;
      continue;
    }

    byte_buffer_t *byteBuffer = byte_buffer_create_from_bytes(size, msgBuffer);
    message_t *message = message_create_from_byte_buffer(byteBuffer);

    printf("[Message TAG %d]: ByteBuffer(Count: %d, Capacity: %d)\n", message->tag, byteBuffer->count, byteBuffer->capacity);

    on_message_received(client, message);

    message_delete(message);

    bzero(msgBuffer, BUFFER_SIZE);
  }

	if(client->room != NULL)
	{
		room_remove_client(client->room, client);
	}

  if(client->user.name != NULL)
  {
    printf("%s left the server\n", client->user.name);
  }

  client_pool_remove(client);

  pthread_detach(pthread_self());

  close(client->sockfd);
  free(client);
}

bool client_send(client_t *client, message_t *message)
{
  // byte_buffer_print_debug(message->buffer);

  byte_buffer_t* buffer = message_to_buffer(message);

  // byte_buffer_print_debug(buffer);

  printf("Sending message with tag %d to client id %d\n", message->tag, client->uid);
  if (send(client->sockfd, buffer->buffer, buffer->count, 0) < 0)
  {
    printf("DEBUG: Error sending message with tag: %d\n", message->tag);
    return false;
  }
  byte_buffer_delete(buffer);
  return true;
}

bool client_is_logged(client_t* client)
{
  return client->user.name != NULL;
}

void client_serialize(client_t* client, lso_writer_t* writer)
{
  lso_writer_write_int32(writer, client->uid);
  lso_writer_write_string(writer, client->user.name, 4);
}

client_t* client_create(struct sockaddr_in address, int connfd)
{
	client_t *cli = (client_t *)malloc(sizeof(client_t));
	cli->address = address;
	cli->sockfd = connfd;
	cli->uid = counter_client++;
	cli->room = NULL;

	pthread_mutex_init(&cli->mutex, NULL);
	pthread_create(&cli->thread, NULL, &client_handler, (void *)cli);
	return cli;
}