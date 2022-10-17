#ifndef __H_CHAT__
#define __H_CHAT__
#include "client.h"

#include <stdio.h>
#include <time.h>

#define MAX_CHAT_TIME_IN_SECONDS 60 * 1

typedef struct chat_t {
  client_t* client1;
  client_t* client2;

  int64_t start_timestamp;
} chat_t;

chat_t* chat_create(client_t* client1, client_t* client2);
int32_t chat_is_over(chat_t* chat);

void chat_close(chat_t* chat);
#endif