
#ifndef __H_MESSAGES__
#define __H_MESSAGES__

#include "buffer.h"
#include "lso_reader.h"
#include "lso_writer.h"
#include "utils.h"

typedef struct {
  int16_t tag;
  byte_buffer_t* buffer;
} message_t;

void message_destroy(message_t* buffer);

message_t* message_create_from_writer(int16_t tag, lso_writer_t* writer);
message_t* message_create_from_byte_buffer(byte_buffer_t* buffer);
message_t* message_create_empty(int16_t tag);

lso_reader_t* message_to_reader(message_t* message);
byte_buffer_t* message_to_buffer(message_t* message);


#endif