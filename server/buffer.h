
#ifndef __H_DBUFFER__
#define __H_DBUFFER__
#include <stdint.h>

typedef struct byte_buffer_t {
  int8_t* buffer;

  int32_t capacity;

  // Number of bytes
  int32_t count;

  int32_t offset;

} byte_buffer_t;

void byte_buffer_delete(byte_buffer_t* bf);

byte_buffer_t* byte_buffer_create(int32_t minCapacity);
byte_buffer_t* byte_buffer_create_from_bytes(int32_t size, int8_t* bytes);

byte_buffer_t* byte_buffer_clone(byte_buffer_t* bf);

void byte_buffer_ensure_size(byte_buffer_t* buffer, int space);

void byte_buffer_print_debug(byte_buffer_t* buffer);

#endif