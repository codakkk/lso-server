#include "messages.h"
#include "buffer.h"
#include "utils.h"
#include "lso_reader.h"
#include <stdlib.h>
#include <string.h>

void message_delete(message_t* message)
{
  if(message == NULL) {
    return;
  }

  if(message->buffer != NULL) 
  {
    byte_buffer_delete(message->buffer);
    message->buffer = NULL;
  }

  free(message);
}

message_t* message_create_from_writer(int16_t tag, lso_writer_t* writer)
{
  message_t* message = malloc(sizeof(message_t));

  message->tag = tag;
  message->buffer = lso_writer_to_byte_buffer(writer);
  return message;
}

message_t* message_create_from_byte_buffer(byte_buffer_t* buffer)
{
  message_t* message = malloc(sizeof(message_t));
  
  message->buffer = byte_buffer_clone(buffer);

  int32_t headerSize = 2;
  // We offset by headerSize because there's tag header payload (headerSize byte)
  message->buffer->offset = buffer->offset + headerSize;
  message->buffer->count = message->buffer->count - headerSize;

  message->tag = read_int16(buffer, buffer->offset);

  return message;
}

message_t* message_create_empty(int16_t tag)
{
  message_t* message = malloc(sizeof(message_t));
  message->buffer = byte_buffer_create(0);
  message->tag = tag;

  return message;
}

lso_reader_t* message_to_reader(message_t* message)
{
  return lso_reader_create(message->buffer);
}

byte_buffer_t* message_to_buffer(message_t* message)
{
  int32_t headerLength = 2;
  int32_t totalLength = message->buffer->count + headerLength;

  byte_buffer_t* buffer = byte_buffer_create(totalLength);
  buffer->count = totalLength;

  write_int16(buffer, 0, message->tag);

  for(int i = 0; i < message->buffer->count; ++i) {
    buffer->buffer[headerLength+i] = message->buffer->buffer[i];
  }

  //memcpy(buffer->buffer + buffer->buffer->offset + 2, message->buffer, message->buffer->count);
  return buffer;
}