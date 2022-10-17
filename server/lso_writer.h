#ifndef __H_LSO_WRITER__
#define __H_LSO_WRITER__

#include <stdbool.h>
#include <stdlib.h>
#include "buffer.h"

typedef struct {
  int32_t position;
  byte_buffer_t* buffer;
} lso_writer_t;

void lso_writer_initialize(lso_writer_t* writer, int minLength);
void lso_writer_destroy(lso_writer_t* writer);

void lso_writer_write_bool(lso_writer_t* writer, bool v);
void lso_writer_write_int8(lso_writer_t* writer, int8_t v);
void lso_writer_write_int16(lso_writer_t* writer, int16_t v);
void lso_writer_write_int32(lso_writer_t* writer, int32_t v);
void lso_writer_write_int64(lso_writer_t* writer, int64_t v);

void lso_writer_write_string(lso_writer_t* writer, char* str);
byte_buffer_t* lso_writer_to_byte_buffer(lso_writer_t* writer);
#endif