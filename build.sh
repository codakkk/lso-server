#!/bin/bash

gcc -pthread -g -o c ./client/main.c
gcc -pthread -g -o s ./server/main.c