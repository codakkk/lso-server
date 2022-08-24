#ifndef __H_CHAT__
#define __H_CHAT__

#include "client.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct chat_t {
    int id;

    pthread_mutex_t mutex;
    pthread_t tid;
    
    struct client_t* clients[2];
};

struct chat_t* chat_create(struct client_t* client1, struct client_t* client2);

void _on_chat_update(void* args);

#endif