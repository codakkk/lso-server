#ifndef __H_UTILS__
#define __H_UTILS__

#include "buffer.h"

#include <stdint.h>

#define MAX(a,b) (((a)>(b))?(a):(b))

void write_int32(byte_buffer_t* buffer, int32_t start, int32_t value);
void write_int16(byte_buffer_t* buffer, int32_t start, int16_t value);
void write_int8(byte_buffer_t* buffer, int32_t start, int8_t value);

int32_t read_int32(byte_buffer_t* buffer, int start);
int16_t read_int16(byte_buffer_t* buffer, int start);
int8_t read_int8(byte_buffer_t* buffer, int start);

#endif