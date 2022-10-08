#ifndef __H_LSO_READER__
#define __H_LSO_READER__

#include <stdlib.h>
#include "buffer.h"

typedef struct {
  int32_t position;
  byte_buffer_t* buffer;
} lso_reader_t;

lso_reader_t* lso_reader_create(byte_buffer_t* buffer);

int8_t lso_reader_read_int8(lso_reader_t* reader);
int16_t lso_reader_read_int16(lso_reader_t* reader);
int32_t lso_reader_read_int32(lso_reader_t* reader);
int32_t lso_reader_read_string(lso_reader_t* reader, char** string);

#endif