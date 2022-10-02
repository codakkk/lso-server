#ifndef __H_LSO_WRITER__
#define __H_LSO_WRITER__

#include <stdlib.h>
#include "buffer.h"

typedef struct {
  int32_t position;
  byte_buffer_t* buffer;
} lso_writer_t;

lso_writer_t* lso_writer_create(int minLength);

void lso_writer_write_int8(lso_writer_t* writer, int8_t v);
void lso_writer_write_int16(lso_writer_t* writer, int16_t v);
void lso_writer_write_int32(lso_writer_t* writer, int32_t v);
void lso_writer_write_string(lso_writer_t* writer, char* str);

#endif