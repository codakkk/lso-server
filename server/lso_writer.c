#include "lso_writer.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>


void lso_writer_initialize(lso_writer_t* writer, int minLength)
{
  writer->position = 0;
  writer->buffer = byte_buffer_create(minLength);
}

/*
  This destroys only internal structure.
   Free should be called on writer if it's dynamically allocated
*/
void lso_writer_destroy(lso_writer_t* writer)
{
	if(writer->buffer == NULL)
	{
		return;
	}
	byte_buffer_delete(writer->buffer);
	writer->buffer = NULL;
}

void lso_writer_write_bool(lso_writer_t* writer, bool v)
{
  byte_buffer_ensure_size(writer->buffer, writer->position + 1);
  
  write_int8(writer->buffer, writer->position++, v == true ? 1 : 0);

  writer->buffer->count = MAX(writer->buffer->count, writer->position);
}

void lso_writer_write_int8(lso_writer_t* writer, int8_t v)
{
  byte_buffer_ensure_size(writer->buffer, writer->position + 1);
  
  write_int8(writer->buffer, writer->position++, v);

  writer->buffer->count = MAX(writer->buffer->count, writer->position);
}

void lso_writer_write_int16(lso_writer_t* writer, int16_t v)
{
  byte_buffer_ensure_size(writer->buffer, writer->position + 2);
  write_int16(writer->buffer, writer->position, v);

  writer->position += 2;

  writer->buffer->count = MAX(writer->buffer->count, writer->position);
}

void lso_writer_write_int32(lso_writer_t* writer, int32_t v)
{
  byte_buffer_ensure_size(writer->buffer, writer->position + 4);
  write_int32(writer->buffer, writer->position, v);

  writer->position += 4;

  writer->buffer->count = MAX(writer->buffer->count, writer->position);
}

void lso_writer_write_int64(lso_writer_t* writer, int64_t v)
{
  byte_buffer_ensure_size(writer->buffer, writer->position + 8);
  write_int64(writer->buffer, writer->position, v);

  writer->position += 8;

  writer->buffer->count = MAX(writer->buffer->count, writer->position);
}

void lso_writer_write_string(lso_writer_t* writer, int8_t* str, int32_t length)
{
	lso_writer_write_int32(writer, length);

  for(int i = 0; i < length; ++i)
	{
		lso_writer_write_int8(writer, str[i]);
  }

	// lso_writer_write_int8(writer, '\0');

  writer->buffer->count = MAX(writer->buffer->count, writer->position);
}

byte_buffer_t* lso_writer_to_byte_buffer(lso_writer_t* writer)
{
  return byte_buffer_clone(writer->buffer);
}