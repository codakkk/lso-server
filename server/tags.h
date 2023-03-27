#ifndef __TAGS_H_
#define __TAGS_H_

#include <stdlib.h>

// Those values must match with Tags.java in Client

extern const int16_t kRequestRoomsTag;
extern const int16_t kRequestRoomsAcceptedTag;

extern const int16_t kJoinRoomRequestTag;
extern const int16_t kJoinRoomAcceptedTag;
extern const int16_t kJoinRoomRefusedTag;
extern const int16_t kJoinRoomNotifyAcceptedTag;

extern const int16_t kMessageReceivedTag;
extern const int16_t kSendMessageTag;

extern const int16_t kLeaveRoomRequestedTag;
extern const int16_t kLeaveRoomTag;

extern const int16_t kSignUpRequestedTag;
extern const int16_t kSignUpRejectedTag;
extern const int16_t kSignUpAcceptedTag;

extern const int16_t kSignInRequestedTag;
extern const int16_t kSignInAcceptedTag;
extern const int16_t kSignInRejectedTag;

extern const int16_t kRoomCreateRequestedTag;
extern const int16_t kRoomCreateAcceptedTag;
extern const int16_t kRoomCreateRejectedTag;
extern const int16_t kRoomClosedTag;

extern const int16_t kJoinRoomRequestedTag;
#endif