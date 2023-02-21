#ifndef __TAGS_H_
#define __TAGS_H_

#include <stdlib.h>

// Those values must match with Tags.java in Client


extern const int16_t JoinRequestTag;
extern const int16_t JoinRequestAcceptedTag;

extern const int16_t RequestRoomsTag;
extern const int16_t RoomsTag;

extern const int16_t SendFirstConfigurationTag;
extern const int16_t FirstConfigurationAcceptedTag;

extern const int16_t kRoomTag;
extern const int16_t kJoinRoomTag;
extern const int16_t kJoinRoomAcceptedTag;
extern const int16_t kJoinRoomRefusedTag;

extern const int16_t kMessageTag;
extern const int16_t kConfirmSentMessageTag;
extern const int16_t kRejectSentMessageTag;

extern const int16_t kLeaveRoomTag;

extern const int16_t kSignUpRequestedTag;
extern const int16_t kSignUpRejectedTag;
extern const int16_t kSignUpAcceptedTag;

extern const int16_t kSignInRequestedTag;
extern const int16_t kSignInAcceptedTag;
extern const int16_t kSignInRejectedTag;

extern const int16_t kRoomCreateTag;
#endif