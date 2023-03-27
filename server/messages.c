#include "messages.h"
#include "lso_writer.h"
#include "room.h"
#include "tags.h"
#include "message.h"
#include "client.h"
#include <string.h>

message_t* create_request_rooms_accepted_message()
{
	lso_writer_t writer;
	lso_writer_initialize(&writer, 1);

	for (int i = 0; i < MAX_ROOMS; ++i)
	{
		room_t* room = gRooms[i];

		if (room == NULL)
			continue;

		room_serialize(&writer, room);
	}

	message_t* message = message_create_from_writer(kRequestRoomsAcceptedTag, &writer);

	// lso_writer_destroy(&writer);
	return message;
}

message_t* create_join_room_accepted_message(room_t* room)
{
	lso_writer_t writer;
	lso_writer_initialize(&writer, 1);
	room_serialize(&writer, room);

	message_t* message = message_create_from_writer(kJoinRoomAcceptedTag, &writer);
	// lso_writer_destroy(&writer);

	return message;
}

message_t* create_join_room_notify_accepted_message(client_t* client)
{
	lso_writer_t writer;
	lso_writer_initialize(&writer, 1);
	client_serialize(client, &writer);

	message_t* message = message_create_from_writer(kJoinRoomNotifyAcceptedTag, &writer);

	// lso_writer_destroy(&writer);

	return message;
}

message_t* create_join_room_refused_message(room_t* room)
{
	message_t *message = message_create_empty(kJoinRoomRefusedTag);
	return message;
}

message_t* create_join_room_requested_message(client_t* client)
{
	lso_writer_t writer;
	lso_writer_initialize(&writer, 1);
	client_serialize(client, &writer);

	message_t* msg = message_create_from_writer(kJoinRoomRequestedTag, &writer);
	// lso_writer_destroy(&writer);

	return msg;
}

message_t* create_leave_room_message(client_t* client)
{
	lso_writer_t writer;
	lso_writer_initialize(&writer, 1);
	client_serialize(client, &writer);
	message_t *message = message_create_from_writer(kLeaveRoomTag, &writer);

	// lso_writer_destroy(&writer);

	return message;
}

message_t* create_send_message_message(client_t* sender, int8_t* messageText, int32_t messageLength)
{
	lso_writer_t writer;
	lso_writer_initialize(&writer, messageLength);
	client_serialize(sender, &writer);
	lso_writer_write_string(&writer, messageText, messageLength);

	message_t* msg = message_create_from_writer(kMessageReceivedTag, &writer);
	// lso_writer_destroy(&writer);

	return msg;
}

message_t* create_room_create_accepted_message(room_t* room)
{
	lso_writer_t writer;
	lso_writer_initialize(&writer, 1);
	room_serialize(&writer, room);

	message_t* message = message_create_from_writer(kRoomCreateAcceptedTag, &writer);

	// lso_writer_destroy(&writer);

	return message;
}