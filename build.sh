#!/bin/bash

gcc -pthread -g -o c ./client/main.c
gcc -pthread -g -o s ./server/main.c ./server/client.c ./server/room.c ./server/client_pool.c ./server/room_pool.c