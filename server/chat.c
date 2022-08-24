#include "chat.h"

#define BUFFER_SZ 2048

int chat_counter = 0;

struct chat_t *chat_create(struct client_t *client1, struct client_t *client2)
{
  struct chat_t *chat = (struct chat_t *)malloc(sizeof(struct chat_t));
  
  chat->clients[0] = client1;
  chat->clients[1] = client2;

  chat->id = chat_counter++;

  pthread_mutex_init(&chat->mutex, NULL);
  pthread_create(&chat->tid, NULL, &_on_chat_update, (void *)chat);
}

void _on_chat_update(void* args)
{
  struct chat_t *chat = (struct chat_t *)args;

  char buff_out[BUFFER_SZ];
  int leave_flag = 0;

  sprintf(buff_out, "Matched with %s\n", chat->clients[0]->name);
  client_send_message(chat->clients[1], buff_out);

  sprintf(buff_out, "Matched with %s\n", chat->clients[1]->name);
  client_send_message(chat->clients[0], buff_out);
  bzero(buff_out, BUFFER_SZ);

  while (chat->clients[0] != NULL && chat->clients[1] != NULL)
  {
    for (int i = 0; i < 2; ++i)
    {
      int leave_flag = 0;
      
      struct client_t *cli = chat->clients[i];

      int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
      if (receive > 0)
      {
        if (strlen(buff_out) > 0)
        {
          if (strcmp(buff_out, "exit_chat\n\0") == 0 || strcmp(buff_out, "exit_chat_AKW\0") == 0)
          {
            printf("%s exit form chat\n", cli->name);
            leave_flag = 1;
            if (strcmp(buff_out, "exit_chat\n\0") == 0)
            {
              client_send_message(cli->chat_with, buff_out);
            }

            client_leave_chat(cli);
          }
          else
          {
            client_send_message(cli->chat_with, buff_out);
          }
        }
      }
      else if (receive == 0 || strcmp(buff_out, "exit") == 0)
      {
        printf("%s exit\n", cli->name);
        sprintf(buff_out, "exit");
        leave_flag = 1;

        client_leave_room(cli);
        client_send_message(cli->chat_with, buff_out);
      }
      else
      {
        printf("ERROR: -1\n");
        leave_flag = 1;
      }

      bzero(buff_out, BUFFER_SZ);

      if (leave_flag)
      {
        bzero(buff_out, BUFFER_SZ);
        client_leave_chat(cli);
        break;
      }
    }
  }
}