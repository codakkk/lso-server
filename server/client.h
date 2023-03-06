#ifndef __H_CLIENT__
#define __H_CLIENT__
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdbool.h>

#include "messages.h"

typedef struct 
{
    int id;
    char* name;
} user_t;

typedef struct client_t
{
    struct sockaddr_in address;

    struct room_t* room;

    pthread_t thread;
    pthread_mutex_t mutex;

    int sockfd;
    int32_t uid;

    user_t* user;
} client_t;

client_t *client_create(struct sockaddr_in address, int connfd);

void client_lock(client_t* client);
void client_unlock(client_t* client);

void *_client_handler(void* args);

bool client_send(client_t* client, message_t* message);
bool client_is_logged(client_t* client);
#endif