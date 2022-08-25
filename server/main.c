#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>

#include "./client.h"
#include "./room.h"
#include "client_pool.h"
#include "room_pool.h"

#define BUFFER_SZ 2048

void print_client_addr(struct sockaddr_in addr)
{
    printf("%d.%d.%d.%d",
           addr.sin_addr.s_addr & 0xff,
           (addr.sin_addr.s_addr & 0xff00) >> 8,
           (addr.sin_addr.s_addr & 0xff0000) >> 16,
           (addr.sin_addr.s_addr & 0xff000000) >> 24);
}




void create_rooms()
{
    printf("Generating rooms...\n");
    room_pool_initialize();
    room_pool_add(room_create("Aldo, Giovanni e Giacomo"));
    room_pool_add(room_create("Sport"));
    room_pool_add(room_create("Barzellette!"));
    room_pool_list_all();
    printf("Rooms generated.\n");
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);

    printf("Listening on %s:%d\n", ip, port);

    int option = 1;
    int listenfd = 0;
    struct sockaddr_in serv_addr;

    /* Socket settings */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port);

    /* Ignore pipe signals */
    signal(SIGPIPE, SIG_IGN);

    if (setsockopt(listenfd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *)&option, sizeof(option)) < 0)
    {
        perror("ERROR: setsockopt failed");
        return EXIT_FAILURE;
    }

    /* Bind */
    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR: Socket binding failed");
        return EXIT_FAILURE;
    }

    /* Listen */
    if (listen(listenfd, 10) < 0)
    {
        perror("ERROR: Socket listening failed");
        return EXIT_FAILURE;
    }

    printf("Chatroom created by Ciro Carandente, Luca Corteccioni, Giovanni Bruno\n");
    create_rooms();
    
    pthread_t tid;
    int connfd;
    struct sockaddr_in cli_addr;
    while (1)
    {
        socklen_t clilen = sizeof(cli_addr);
        connfd = accept(listenfd, (struct sockaddr *)&cli_addr, &clilen);

        /* Check if max clients is reached */
        if (client_pool_is_full())
        {
            printf("Max clients reached. Rejected: ");
            print_client_addr(cli_addr);
            printf(":%d\n", cli_addr.sin_port);
            close(connfd);
            continue;
        }
        
        client_pool_lock();
        client_pool_add(create_client(cli_addr, connfd));
        client_pool_unlock();

        room_pool_list_all();

        /* Reduce CPU usage */
        sleep(1);
    }

    return EXIT_SUCCESS;
}
