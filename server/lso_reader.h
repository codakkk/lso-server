#ifndef __H_LSO_READER__
#define __H_LSO_READER__

#include "buffer.h"

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  int32_t position;
  byte_buffer_t* buffer;
} lso_reader_t;

lso_reader_t* lso_reader_create(byte_buffer_t* buffer);
void lso_reader_delete(lso_reader_t* reader);

bool lso_reader_read_bool(lso_reader_t* reader);
int8_t lso_reader_read_int8(lso_reader_t* reader);
int16_t lso_reader_read_int16(lso_reader_t* reader);
int32_t lso_reader_read_int32(lso_reader_t* reader);
int32_t lso_reader_read_string(lso_reader_t* reader, char** string);

#endif