cmake_minimum_required(VERSION 3.25)
project(lso_server C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(server)

add_executable(lso_server
        server/buffer.c
        server/buffer.h
        server/client.c
        server/client.h
        server/client_pool.c
        server/client_pool.h
        server/database.c
        server/database.h
        server/lso_reader.c
        server/lso_reader.h
        server/lso_writer.c
        server/lso_writer.h
        server/main.c
        server/message.c
        server/message.h
        server/room.c
        server/room.h
        server/tags.c
        server/tags.h
        server/utils.c
        server/utils.h server/messages.h server/messages.c)

target_link_libraries(lso_server mysqlclient)
