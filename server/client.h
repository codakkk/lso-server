#ifndef __H_CLIENT__
#define __H_CLIENT__
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdbool.h>

struct client_t {
    struct sockaddr_in address;

    struct client_t* chat_with;
    struct client_t* last_chat_with;
    struct room_t* room;

    pthread_t thread;

    int sockfd;
    int uid;
    char name[32];
};


struct client_t* create_client(struct sockaddr_in address, int connfd);

bool client_send_message(struct client_t* client, char* message);
void client_enter_room(struct client_t* client, struct room_t* room);

void client_leave_chat(struct client_t* client);
void client_leave_room(struct client_t* client);

bool client_is_free(struct client_t* client);

void* _client_handler(void* args);
#endif