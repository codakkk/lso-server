#ifndef __H_CLIENT__
#define __H_CLIENT__
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

typedef struct{
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[32];
} client_t;

int counter = 0;

client_t* create_client(struct sockaddr_in address, int connfd) 
{ 
  client_t *cli = (client_t *)malloc(sizeof(client_t));
  cli->address = address;
  cli->sockfd = connfd;
  cli->uid = counter++;

  return cli;
}

#endif