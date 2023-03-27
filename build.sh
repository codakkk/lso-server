#!/bin/bash

gcc -pthread -g -o s ./server/main.c ./server/client.c ./server/room.c ./server/client_pool.c ./server/utils.c ./server/buffer.c ./server/message.c ./server/messages.c ./server/lso_writer.c ./server/lso_reader.c ./server/tags.c ./server/database.c -lmysqlclient