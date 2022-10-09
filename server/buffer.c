#include "buffer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 
  This destroys only internal structure.
  Free should be called on bf if it's dynamically allocated
*/
void byte_buffer_destroy(byte_buffer_t* bf)
{
  if(bf->buffer != NULL)
  {
    free(bf->buffer);
    bf->buffer = NULL;
  }
}

byte_buffer_t* byte_buffer_create(int32_t minCapacity) 
{
  byte_buffer_t* buffer = malloc(sizeof(byte_buffer_t));
  
  buffer->buffer = malloc(sizeof(int8_t) * minCapacity);
  buffer->capacity = minCapacity;

  buffer->offset = 0;
  buffer->count = 0;

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
  byte_buffer_t* buffer = malloc(sizeof(byte_buffer_t));
  
  buffer->buffer = bf->buffer;
  buffer->capacity = bf->capacity;

  buffer->offset = bf->offset;
  buffer->count = bf->count;
  return buffer;
}

void byte_buffer_ensure_size(byte_buffer_t* buffer, int space)
{
  if(space <= buffer->capacity) 
  {
    return;
  }

  int8_t* newBuffer = malloc(sizeof(int8_t) * space);
  memcpy(newBuffer, buffer->buffer, buffer->capacity);

  buffer->capacity = space;
}

void byte_buffer_print_debug(byte_buffer_t* buffer)
{
  for(int i = 0; i < buffer->count; ++i)
  {
    printf("%d,", buffer->buffer[i]);
  }
  printf("\n");
}