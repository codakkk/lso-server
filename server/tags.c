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

const int16_t kMessageTag = 11;
const int16_t kConfirmSentMessageTag = 12;
const int16_t kRejectSentMessageTag = 13;

const int16_t kLeaveRoomTag = 14;

const int16_t kSignUpRequestedTag = 20;
const int16_t kSignUpAcceptedTag = 21;
const int16_t kSignUpRejectedTag = 22;

const int16_t kSignInRequestedTag = 30;
const int16_t kSignInAcceptedTag = 31;
const int16_t kSignInRejectedTag = 32;

const int16_t kRoomCreateTag = 40;