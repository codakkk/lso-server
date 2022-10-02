#ifndef __H_CLIENT__
#define __H_CLIENT__
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdbool.h>

#include "messages.h"

struct client_t {
    struct sockaddr_in address;

    struct client_t* chat_with;
    struct client_t* last_chat_with;
    struct room_t* room;

    pthread_t thread;
    pthread_mutex_t mutex;

    int sockfd;
    int uid;
    char* name;
};


struct client_t* client_create(struct sockaddr_in address, int connfd);

void client_lock(struct client_t* client);
void client_unlock(struct client_t* client);

bool client_send_message(struct client_t* client, char* message);
void client_set_room(struct client_t* client, struct room_t* room);
void client_set_chat_with(struct client_t* client, struct client_t* with);

bool client_is_free(struct client_t* client);

void* _client_handler(void* args);

bool client_send(struct client_t* client, message_t* message);

#endif