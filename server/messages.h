#ifndef H_MESSAGES__
#define H_MESSAGES__

#include "lso_writer.h"
#include "room.h"
#include "tags.h"
#include "message.h"
#include "client.h"

/*
 * Room Messages
 * */

message_t* create_request_rooms_accepted_message();
message_t* create_join_room_accepted_message(room_t* room);
message_t* create_join_room_notify_accepted_message(client_t* client);
message_t* create_join_room_refused_message(room_t* room);
message_t* create_join_room_requested_message(client_t* client);
message_t* create_leave_room_message(client_t* client);
message_t* create_room_create_accepted_message(room_t* room);

message_t* create_send_message_message(client_t* sender, int8_t* messageText, int32_t messageLength);

#endif