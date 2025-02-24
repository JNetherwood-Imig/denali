#pragma once

#include "dwl/defines.h"

typedef enum MessageArgType {
    MESSAGE_ARG_TYPE_INT = 'i',
    MESSAGE_ARG_TYPE_UINT = 'u',
    MESSAGE_ARG_TYPE_FIXED = 'f',
    MESSAGE_ARG_TYPE_OBJECT = 'o',
    MESSAGE_ARG_TYPE_NEW_ID = 'n',
    MESSAGE_ARG_TYPE_STRING = 's',
    MESSAGE_ARG_TYPE_ARRAY = 'a',
    MESSAGE_ARG_TYPE_FD = 'd',
    MESSAGE_ARG_TYPE_ENUM = 'e',
} MessageArgType;

typedef struct MessageHeader {
    u32 obj_id;
    u16 opcode;
    u16 length;
} MessageHeader;

typedef struct Message {
    MessageHeader head;
    u8* buf;
    u8* ptr;
} Message;

void message_destroy(Message* m);

i32 message_read_i32(Message* m);

u32 message_read_u32(Message* m);

const char* message_read_str(Message* m);
