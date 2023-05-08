#!/bin/bash

gcc -pthread -g -o s main.c client.c room.c client_pool.c utils.c buffer.c message.c messages.c lso_writer.c lso_reader.c tags.c database.c -lmysqlclient