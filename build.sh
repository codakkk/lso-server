#!/bin/bash

gcc -pthread -g -o client client.c
gcc -pthread -g -o server server.c client.h