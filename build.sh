#!/bin/bash

gcc -pthread -g -o c ./client/main.c
gcc -pthread -g -o s ./server/main.c ./server/client.c ./server/room.c ./server/client_pool.c ./server/room_pool.c ./server/utils.c ./server/buffer.c ./server/messages.c ./server/lso_writer.c ./server/lso_reader.c ./server/tags.c