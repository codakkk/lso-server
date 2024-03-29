#include "tags.h"

#include <stdint.h>

const int16_t kRequestRoomsTag = 2;
const int16_t kRequestRoomsAcceptedTag = 3;

const int16_t kJoinRoomRequestTag = 7;
const int16_t kJoinRoomAcceptedTag = 8;
const int16_t kJoinRoomRefusedTag = 9;
const int16_t kJoinRoomNotifyAcceptedTag = 10;

const int16_t kMessageReceivedTag = 11;
const int16_t kSendMessageTag = 12;

const int16_t kLeaveRoomRequestedTag = 14;
const int16_t kLeaveRoomTag = 15;


const int16_t kSignUpRequestedTag = 20;
const int16_t kSignUpAcceptedTag = 21;
const int16_t kSignUpRejectedTag = 22;

const int16_t kSignInRequestedTag = 30;
const int16_t kSignInAcceptedTag = 31;
const int16_t kSignInRejectedTag = 32;

const int16_t kRoomCreateRequestedTag = 40;
const int16_t kRoomCreateAcceptedTag = 41;
const int16_t kRoomClosedTag = 43;

const int16_t kJoinRoomRequestedTag = 50;