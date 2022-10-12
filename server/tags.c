#include "tags.h"

#include <stdint.h>

const int16_t JoinRequestTag = 0;
const int16_t JoinRequestAcceptedTag = 1;

const int16_t RequestRoomsTag = 2;
const int16_t RoomsTag = 3;

const int16_t SendFirstConfigurationTag = 4;
const int16_t FirstConfigurationAcceptedTag = 5;

const int16_t kRoomTag = 6;
const int16_t kJoinRoomTag = 7;
const int16_t kJoinRoomAcceptedTag = 8;
const int16_t kJoinRoomRefusedTag = 9;

const int16_t kOnMatchTag = 10;

const int16_t kSendMessageTag = 11;
const int16_t kConfirmSentMessageTag = 12;
const int16_t kRejectSentMessageTag = 13;