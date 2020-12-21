#pragma once

#include <stdint.h>

#define CAN_MESSAGE_ID  0x123

#define MESSAGE_ID_ON_OFF       (0x1)
#define MESSAGE_ID_SPEED        (0x2)
#define MESSAGE_ID_MODE         (0x3)
#define MESSAGE_ID_MODE_WHITE   (0x4)
#define MESSAGE_ID_BRIGHTNESS   (0x5)
#define MESSAGE_ID_INTENSITY    (0x6)
#define MESSAGE_ID_COLOR        (0x7)
#define MESSAGE_ID_SYNC_STATE   (0x8)

typedef struct
{
    uint8_t id;
    uint8_t value0;
    uint8_t value1;
    uint8_t value2;
} can_message_t;