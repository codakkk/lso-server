#include "utils.h"
#include <stdint.h>

void write_int32(byte_buffer_t* buffer, int32_t start, int32_t value) 
{
  buffer->buffer[start] = value >> 24;
  buffer->buffer[start+1] = value >> 16;
  buffer->buffer[start+2] = value >> 8;
  buffer->buffer[start+3] = value;
}

void write_int16(byte_buffer_t* buffer, int32_t start, int16_t value) 
{
  buffer->buffer[start] = value >> 8;
  buffer->buffer[start+1] = value;
}


void write_int8(byte_buffer_t* buffer, int32_t start, int8_t value) 
{
  buffer->buffer[start] = value;
}

int32_t read_int32(byte_buffer_t* buffer, int start)
{
  return (int32_t)(buffer->buffer[start] << 24 | buffer->buffer[start+1] << 16 | buffer->buffer[start+2] << 8 | buffer->buffer[start+3]);
}

int16_t read_int16(byte_buffer_t* buffer, int start)
{
  return (int16_t)(buffer->buffer[start] << 8 | buffer->buffer[start+1]);
}

int8_t read_int8(byte_buffer_t* buffer, int start)
{
  return (int8_t)(buffer->buffer[start]);
}