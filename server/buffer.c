#include "buffer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void byte_buffer_delete(byte_buffer_t* bf)
{
  if(bf->buffer != NULL)
  {
    free(bf->buffer);
    bf->buffer = NULL;
  }

  free(bf);
}

byte_buffer_t* byte_buffer_create(int32_t minCapacity) 
{
  byte_buffer_t* buffer = malloc(sizeof(byte_buffer_t));
  
  buffer->buffer = malloc(sizeof(int8_t) * (minCapacity < 1 ? 1 : minCapacity));
  buffer->capacity = minCapacity;

  buffer->offset = 0;
  buffer->count = 0;

	for(int32_t i = 0; i < minCapacity; ++i)
	{
		buffer->buffer[i] = 0;
	}

  return buffer;
}

byte_buffer_t* byte_buffer_create_from_bytes(int32_t size, int8_t* bytes)
{
  byte_buffer_t* byteBuffer = byte_buffer_create(size);
  memcpy(byteBuffer->buffer, bytes, size);
  byteBuffer->count = size;
  return byteBuffer;
}

byte_buffer_t* byte_buffer_clone(byte_buffer_t* bf) 
{
  byte_buffer_t* buffer = byte_buffer_create_from_bytes(bf->capacity, bf->buffer);
  buffer->offset = bf->offset;
  buffer->count = bf->count;

  // buffer->buffer = bf->buffer;
  // buffer->capacity = bf->capacity;

  // buffer->offset = bf->offset;
  // buffer->count = bf->count;
  return buffer;
}

void byte_buffer_ensure_size(byte_buffer_t* byteBuffer, int32_t space)
{
  if(space <= byteBuffer->capacity)
  {
    return;
  }

  int8_t* newBuffer = malloc(sizeof(int8_t) * space);
  memcpy(newBuffer, byteBuffer->buffer, byteBuffer->capacity);

	free(byteBuffer->buffer);

	byteBuffer->buffer = newBuffer;
	byteBuffer->capacity = space;
}

void byte_buffer_print_debug(byte_buffer_t* buffer)
{
  for(int i = 0; i < buffer->count; ++i)
  {
    printf("%d,", buffer->buffer[i]);
  }
  printf("\n");
}