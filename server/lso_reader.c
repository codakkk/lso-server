#include "lso_reader.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

lso_reader_t* lso_reader_create(byte_buffer_t* buffer)
{
  lso_reader_t* reader = malloc(sizeof(lso_reader_t));
  reader->buffer = buffer;
  reader->position = 0;

  return reader;
}

void lso_reader_delete(lso_reader_t* reader)
{
  if(reader == NULL) {
    return;
  }

  free(reader);
}

bool lso_reader_read_bool(lso_reader_t* reader)
{
  if(reader->position >= reader->buffer->count) 
  {
    printf("Failed reading bool.\n");
    return false;
  }

  return read_int8(reader->buffer, reader->buffer->offset + reader->position++);
}

int8_t lso_reader_read_int8(lso_reader_t* reader) 
{
  if(reader->position >= reader->buffer->count) 
  {
    printf("Failed reading int8.\n");
    return 0;
  }

  return read_int8(reader->buffer, reader->buffer->offset + reader->position++);
}

int16_t lso_reader_read_int16(lso_reader_t* reader) 
{
  if(reader->position+2 > reader->buffer->count) 
  {
    printf("Failed reading int16.\n");
    return 0;
  }

  int16_t v = read_int16(reader->buffer, reader->buffer->offset + reader->position);
  reader->position += 2;
  return v;
}

int32_t lso_reader_read_int32(lso_reader_t* reader) 
{
  if(reader->position+4 > reader->buffer->count) 
  {
    printf("Failed reading int32.\n");
    return 0;
  }

  int32_t v = read_int32(reader->buffer, reader->buffer->offset + reader->position);
  reader->position += 4;
  return v;
}

int32_t lso_reader_read_string(lso_reader_t* reader, char** string)
{
  if(reader->position + 4 > reader->buffer->count)
  {
    return -1;
  }

  int32_t length = read_int32(reader->buffer, reader->buffer->offset + reader->position);

  *string = malloc(sizeof(char) * length);
  for(int i = 0; i < length; ++i) 
  {
    (*string)[i] = read_int8(reader->buffer, reader->buffer->offset + reader->position + 4 + i);
  }

  reader->position += 4 + length;

  return length;
}