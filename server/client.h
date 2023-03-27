#ifndef __H_CLIENT__
#define __H_CLIENT__
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdbool.h>

#include "message.h"

#define MAX_USER_NAME 32

typedef struct 
{
    int id;
    int8_t* name;
} user_t;

typedef struct client_t
{
    struct sockaddr_in address;

    struct room_t* room;

    pthread_t thread;
    pthread_mutex_t mutex;

    int sockfd;
    int32_t uid;

    user_t user;
} client_t;

client_t *client_create(struct sockaddr_in address, int connfd);

bool client_send(client_t* client, message_t* message);
bool client_is_logged(client_t* client);

void client_serialize(client_t* client, lso_writer_t* writer);
#endif