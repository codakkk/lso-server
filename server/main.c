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

#include "./room.h"

#define MAX_ROOMS 3
#define MAX_CLIENTS 5
#define BUFFER_SZ 2048

static _Atomic unsigned int cli_count = 0;
static int uid = 10;

client_t *clients[MAX_CLIENTS];
room_t *rooms[MAX_ROOMS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void str_overwrite_stdout()
{
    printf("\r%s", "> ");
    fflush(stdout);
}

void str_trim_lf(char *arr, int length)
{
    int i;
    for (i = 0; i < length; i++)
    { // trim \n
        if (arr[i] == '\n')
        {
            arr[i] = '\0';
            break;
        }
    }
}

void print_client_addr(struct sockaddr_in addr)
{
    printf("%d.%d.%d.%d",
           addr.sin_addr.s_addr & 0xff,
           (addr.sin_addr.s_addr & 0xff00) >> 8,
           (addr.sin_addr.s_addr & 0xff0000) >> 16,
           (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

/* Add clients to queue */
void queue_add(client_t *cl)
{
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (!clients[i])
        {
            clients[i] = cl;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

/* Remove clients to queue */
void queue_remove(int uid)
{
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i])
        {
            if (clients[i]->uid == uid)
            {
                clients[i] = NULL;
                break;
            }
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

/* Send message to all clients except sender */
void send_message(char *s, int uid)
{
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i])
        {
            if (clients[i]->uid != uid)
            {
                if (write(clients[i]->sockfd, s, strlen(s)) < 0)
                {
                    perror("ERROR: write to descriptor failed");
                    break;
                }
            }
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

/* Send message to a client */
void send_message_to(char *s, int uid)
{
    pthread_mutex_lock(&clients_mutex);

    if (uid != -1)
    {
        for (int i = 0; i < MAX_CLIENTS; ++i)
        {
            if (clients[i])
            {
                if (clients[i]->uid == uid)
                {
                    if (write(clients[i]->sockfd, s, strlen(s)) < 0)
                    {
                        perror("ERROR: write to descriptor failed");
                        break;
                    }
                }
            }
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void print_rooms(int uid)
{
    char buff_out[BUFFER_SZ];

    sprintf(buff_out, "Select your room:\n");
    send_message_to(buff_out, uid);

    for (int i = 0; i < MAX_ROOMS; i++)
    {
        if (rooms[i])
        {
            sprintf(buff_out, "%d) %s %d/%d.\n", i, rooms[i]->channelName, count_in_room(rooms[i]), MAX_CLIENTS_PER_ROOM);
            send_message_to(buff_out, uid);
        }
    }
}

void start_chat(client_t* cli)
{
    
    char buff_out[BUFFER_SZ];
    int leave_flag = 0;

    sprintf(buff_out, "Matched with %s\n", cli->name);
    send_message_to(buff_out, cli->chat_uid);
    bzero(buff_out, BUFFER_SZ);

    while (cli->not_exit)
    {
        if (leave_flag)
        {
            bzero(buff_out, BUFFER_SZ);
            cli->chat_uid = -1;
            break;
        }

        int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
        if (receive > 0)
        {
            if(strlen(buff_out) > 0)
            {
                if ( strcmp(buff_out, "exit_room\n\0") == 0 
                    || strcmp(buff_out, "exit_chat\n\0") == 0
                    || strcmp(buff_out, "exit_chat_AKW\0") == 0)
                {    
                    if (strcmp(buff_out, "exit_chat\n\0") == 0 || strcmp(buff_out, "exit_chat_AKW\0") == 0)
                    {
                        printf("%s exit form chat\n", cli->name);
                        leave_flag = 1; 
                        if (strcmp(buff_out, "exit_chat\n\0") == 0)
                        {
                            send_message_to(buff_out, cli->chat_uid);
                        }
                        cli->chat_uid = -1;
                        cli->is_free = true;
                    }
                    else if (strcmp(buff_out, "exit_room\n\0") == 0)
                    {
                        printf("%s exit form room", cli->name);
                        leave_flag = 1;
                        cli->chat_uid = -1;
                        cli->not_exit = 0;
                    }
                    
                }
                else
                {
                    send_message_to(buff_out, cli->chat_uid);
                }
            }
        } 
        else if (receive == 0 || strcmp(buff_out, "exit") == 0)
        {
            printf("%s exit\n", cli->name);
            sprintf(buff_out, "exit");
            leave_flag = 1;
            cli->not_exit = 0;
            send_message_to(buff_out, cli->chat_uid);
        } 
        else 
        {
            printf("ERROR: -1\n");
            leave_flag = 1;
        }

        bzero(buff_out, BUFFER_SZ);
    }

}

void find_match(room_t* room, client_t* cli) 
{
    while (cli->is_free || cli->not_exit)
    {   
        if (!cli->not_exit)
        {
            break;
        }
        
        pthread_mutex_lock(&room_mutex);

        for(int i=0; i < MAX_CLIENTS_PER_ROOM; ++i)
        {
            if(room->clients[i] && cli->is_free)
            {
                if(room->clients[i]->uid != cli->uid && room->clients[i]->is_free && room->clients[i]->uid != cli->last_chat_uid)
                {
                    // Set uid of the client matched for the chat
                    room->clients[i]->chat_uid = cli->uid;
                    cli->chat_uid = room->clients[i]->uid;

                    // Set uid of the last client matched
                    room->clients[i]->last_chat_uid = cli->uid;
                    cli->last_chat_uid = room->clients[i]->uid;

                    // Set clients not free
                    room->clients[i]->is_free = false;
                    cli->is_free = false;
                    printf("Matched %s and %s\n", cli->name, room->clients[i]->name);
                }
            }
        }

        pthread_mutex_unlock(&room_mutex);

        if (!cli->is_free)
        {
            start_chat(cli);
        }
    }
}

void handle_chat(room_t* room, client_t* cli)
{
    char buff_out[BUFFER_SZ];
    int leave_flag = 0;

    printf("%s has joined the room %s\n", cli->name, room->channelName);


    while (1)
    {
        find_match(room, cli);

        if (!cli->not_exit)
        {
            break;
        }
    }
    remove_client_room(room, cli->uid);

}

/* Handle all communication with the client */
void *handle_client(void *arg)
{
    char buff_out[BUFFER_SZ];
    char name[32];
    int leave_flag = 0;

    cli_count++;
    client_t *cli = (client_t *)arg;

    // Name
    if (recv(cli->sockfd, name, 32, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 32 - 1)
    {
        printf("Didn't enter the name.\n");
        leave_flag = 1;
    }
    else
    {
        strcpy(cli->name, name);
        printf("%s has joined\n", cli->name);
    }

    bzero(buff_out, BUFFER_SZ);

    print_rooms(cli->uid);

    while (1)
    {
        if (leave_flag)
        {
            printf("%s has left the server\n", cli->name);
            break;
        }

        int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
        if (receive > 0)
        {
            if (strlen(buff_out) > 0)
            {
                if (strcmp(buff_out, "0") || strcmp(buff_out, "1") || strcmp(buff_out, "2"))
                {
                    int selected_room = atoi(buff_out);
                    add_client_room(rooms[selected_room], cli);
                    // printf("Room selected %d %s\n", selected_room, buff_out);
                    send_message_to("Waiting for pairing\n", cli->uid);
                    bzero(buff_out, BUFFER_SZ);
                    handle_chat(rooms[selected_room], cli);
                    leave_flag = 1;
                }
                else
                {
                    send_message_to("No room found, retry\n", cli->uid);
                }
            }
        }
        else if (receive == 0 || strcmp(buff_out, "exit") == 0)
        {
            leave_flag = 1;
        }
        else
        {
            printf("ERROR: -1\n");
            leave_flag = 1;
        }

        bzero(buff_out, BUFFER_SZ);
    }

    /* Delete client from queue and yield thread */
    close(cli->sockfd);
    queue_remove(cli->uid);
    free(cli);
    cli_count--;
    pthread_detach(pthread_self());

    return NULL;
}

void create_rooms()
{
    rooms[0] = create_room("Aldo, Giovanni e Giacomo");
    rooms[1] = create_room("Sport");
    rooms[2] = create_room("Barzellette!");
}

void logic_loop(int listenfd) 
{
    pthread_t tid;
    int connfd;
    struct sockaddr_in cli_addr;
    while (1)
    {
        socklen_t clilen = sizeof(cli_addr);
        connfd = accept(listenfd, (struct sockaddr *)&cli_addr, &clilen);

        /* Check if max clients is reached */
        if ((cli_count + 1) == MAX_CLIENTS)
        {
            printf("Max clients reached. Rejected: ");
            print_client_addr(cli_addr);
            printf(":%d\n", cli_addr.sin_port);
            close(connfd);
            continue;
        }

        /* Client settings */
        client_t *cli = create_client(cli_addr, connfd);

        /* Add client to the queue and fork thread */
        queue_add(cli);
        pthread_create(&tid, NULL, &handle_client, (void *)cli);

        /* Reduce CPU usage */
        sleep(1);
    }
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
    printf("Porta: %d", port);
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

    printf("=== WELCOME TO THE CHATROOM ===\n");
    create_rooms();
    logic_loop(listenfd);

    return EXIT_SUCCESS;
}