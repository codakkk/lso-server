#ifndef __H_CLIENT__
#define __H_CLIENT__
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct {
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[32];
    bool is_free;
    int chat_uid;
    int last_chat_uid;
    int not_exit;
} client_t;

int counter_client = 0;

client_t* create_client(struct sockaddr_in address, int connfd) 
{ 
  client_t *cli = (client_t *)malloc(sizeof(client_t));
  cli->address = address;
  cli->sockfd = connfd;
  cli->uid = counter_client++;
  cli->is_free = 1;
  cli->chat_uid = -1;
  cli->last_chat_uid = -1;
  cli->not_exit = 1;

  return cli;
}

#endif