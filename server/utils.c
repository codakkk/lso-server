#include "utils.h"
#include <stdint.h>

void write_int64(byte_buffer_t* buffer, int32_t start, int64_t value) 
{
  buffer->buffer[start + 0] = (int8_t)(value >> 56);
  buffer->buffer[start + 1] = (int8_t)(value >> 48);
  buffer->buffer[start + 2] = (int8_t)(value >> 40);
  buffer->buffer[start + 3] = (int8_t)(value >> 32);
  buffer->buffer[start + 4] = (int8_t)(value >> 24);
  buffer->buffer[start + 5] = (int8_t)(value >> 16);
  buffer->buffer[start + 6] = (int8_t)(value >> 8);
  buffer->buffer[start + 7] = (int8_t)(value);
}

void write_int32(byte_buffer_t* buffer, int32_t start, int32_t value) 
{
  buffer->buffer[start] = (int8_t)(value >> 24);
  buffer->buffer[start+1] = (int8_t)(value >> 16);
  buffer->buffer[start+2] = (int8_t)(value >> 8);
  buffer->buffer[start+3] = (int8_t)(value);
}

void write_int16(byte_buffer_t* buffer, int32_t start, int16_t value) 
{
  buffer->buffer[start] = (int8_t)(value >> 8);
  buffer->buffer[start+1] = (int8_t)(value);
}

void write_int8(byte_buffer_t* buffer, int32_t start, int8_t value) 
{
  buffer->buffer[start] = value;
}

int64_t read_int64(byte_buffer_t* buffer, int start)
{
  int8_t* bytes = buffer->buffer;
  int64_t v1 = ((int64_t)bytes[start + 0]) << 56;
  int64_t v2 = ((int64_t)bytes[start + 1]) << 48;
  int64_t v3 = ((int64_t)bytes[start + 2]) << 40;
  int64_t v4 = ((int64_t)bytes[start + 3]) << 32;
  int64_t v5 = ((int64_t)bytes[start + 4]) << 24;
  int64_t v6 = ((int64_t)bytes[start + 5]) << 16;
  int64_t v7 = ((int64_t)bytes[start + 6]) << 8;
  int64_t v8 = (int64_t)bytes[start + 7];
  return (int64_t)(v1 | v2 | v3 | v4 | v5 | v6 | v7 | v8);
}

int32_t read_int32(byte_buffer_t* buffer, int start)
{
  return (int32_t)(((int32_t)buffer->buffer[start] << 24) | ((int32_t)buffer->buffer[start+1] << 16) | ((int32_t)buffer->buffer[start+2] << 8) | ((int32_t)buffer->buffer[start+3]));
}

int16_t read_int16(byte_buffer_t* buffer, int start)
{
  return (int16_t)(((int16_t)buffer->buffer[start] << 8) | (int16_t)buffer->buffer[start+1]);
}

int8_t read_int8(byte_buffer_t* buffer, int start)
{
  return (int8_t)((int8_t)buffer->buffer[start]);
}