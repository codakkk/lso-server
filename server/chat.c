#include "chat.h"
#include "tags.h"

#include <pthread.h>

chat_t* chat_create(client_t* client1, client_t* client2)
{
  if(client1 == NULL || client2 == NULL)
  {
    printf("Trying to instantiate chat with client1 or client2 = NULL");
    return NULL;
  }
  chat_t* chat = (chat_t*)malloc(sizeof(chat_t));

  chat->client1 = client1;
  chat->client2 = client2;
  chat->start_timestamp = (int64_t)time(NULL);

  printf("Timestamp: %d\n", chat->start_timestamp);

  // Send message to c1
  lso_writer_t writer;
  lso_writer_initialize(&writer, 8);
  lso_writer_write_int32(&writer, client2->uid);
  lso_writer_write_string(&writer, client2->name);
  lso_writer_write_int64(&writer, chat->start_timestamp);
  lso_writer_write_int64(&writer, chat->start_timestamp + MAX_CHAT_TIME_IN_SECONDS);
  lso_writer_write_int32(&writer, MAX_CHAT_TIME_IN_SECONDS);

  message_t* message = message_create_from_writer(kOnMatchTag, &writer);
  client_send(client1, message);

  message_delete(message);

  // Send message to c2
  lso_writer_t c2Writer;
  lso_writer_initialize(&c2Writer, 8);
  lso_writer_write_int32(&c2Writer, client1->uid);
  lso_writer_write_string(&c2Writer, client1->name);
  lso_writer_write_int64(&c2Writer, chat->start_timestamp);
  lso_writer_write_int64(&c2Writer, chat->start_timestamp + MAX_CHAT_TIME_IN_SECONDS);
  lso_writer_write_int32(&c2Writer, MAX_CHAT_TIME_IN_SECONDS);

  message = message_create_from_writer(kOnMatchTag, &c2Writer);
  client_send(client2, message);
  
  message_delete(message);

  return chat;
}

void chat_delete(chat_t* chat)
{
  if(chat == NULL) {
    return;
  }

  free(chat);
}

int32_t chat_is_over(chat_t* chat)
{
  return (time(NULL) - chat->start_timestamp) >= MAX_CHAT_TIME_IN_SECONDS;
}

void chat_close(chat_t* chat)
{
  if(chat == NULL) return;

  printf("Chat between %s and %s closed.", chat->client1->name, chat->client2->name);

  chat->client1->current_chat = NULL;
  chat->client2->current_chat = NULL;  
}