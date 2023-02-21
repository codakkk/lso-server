#ifndef __H_CLIENT__
#define __H_CLIENT__
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdbool.h>

#include "messages.h"

typedef struct client_t
{
    struct sockaddr_in address;

    struct room_t *room;

    pthread_t thread;
    pthread_mutex_t mutex;

    int sockfd;
    int uid;
    char *name;
} client_t;

client_t *client_create(struct sockaddr_in address, int connfd);

void client_lock(client_t *client);
void client_unlock(client_t *client);

void *_client_handler(void *args);

bool client_send(client_t *client, message_t *message);

#endif