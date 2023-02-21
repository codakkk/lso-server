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
#include "utils.h"

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
    gRooms[0] = room_create("Aldo, Giovanni e Giacomo");
    gRooms[1] = room_create("Sport");
    gRooms[2] = room_create("Barzellette!");

    printf("Rooms generated.\n");
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("64 bits test: \n");

        lso_writer_t writer;
        lso_writer_initialize(&writer, 8);
        lso_writer_write_int64(&writer, 10L);

        byte_buffer_print_debug(writer.buffer);
        
        if(read_int64(writer.buffer, 0) == 10L) 
        {
            printf("The same\n");
        } else 
        {
            printf("Not the same\n");
        }

        /*int32_t v1 = 100;
        int16_t v2 = 300;

        printf("Using ByteBuffer: \n");
        bf = byte_buffer_create(6);
        write_int32(bf, 0, v1);
        write_int16(bf, 4, v2);
        printf("%d == %d - %d == %d\n", v1, read_int32(bf, 0), v2, read_int16(bf, 4));
        printf("==================\n\n");

        printf("Using Writer with manual reading: \n");
        
        lso_writer_t writer;
        lso_writer_initialize(&writer, 6);
        
        lso_writer_write_int32(&writer, v1);
        lso_writer_write_int16(&writer, v2);
        printf("%d == %d - %d == %d\n", v1, read_int32(writer.buffer, 0), v2, read_int16(writer.buffer, 4));

        printf("Writer bytes wrote: %d. Capacity: %d\n", writer.buffer->count, writer.buffer->capacity);

        printf("==================\n\n");

        printf("Using Writer using reader: \n");

        lso_reader_t* reader = lso_reader_create(writer.buffer);
        
        int32_t reader32 = lso_reader_read_int32(reader);
        int16_t reader16 = lso_reader_read_int16(reader);
        printf("%d == %d - %d == %d\n", v1, reader32, v2, reader16);
        printf("==================\n\n");*/

        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = "0.0.0.0";
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

    printf("Multi-Chat created by Ciro Carandente for LSO\n");
    create_rooms();

    while (1)
    {
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int connfd = accept(listenfd, (struct sockaddr *)&cli_addr, &clilen);

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
        client_t* client = client_create(cli_addr, connfd);
        client_pool_add(client);
        client_pool_unlock();

        /* Reduce CPU usage */
        sleep(1);
    }

    return EXIT_SUCCESS;
}
