#pragma once

#include "dwl/defines.h"
#include "dwl/protocol.h"

#define MESSAGE_MAX_ARGS 16U

typedef enum MessageArgType {
    MESSAGE_ARG_TYPE_NEW_ID,
    MESSAGE_ARG_TYPE_OBJECT,
    MESSAGE_ARG_TYPE_UINT,
    MESSAGE_ARG_TYPE_STRING,
    MESSAGE_ARG_TYPE_MAX_ENUM,
} MessageArgType;

typedef struct MessageSignature {
    u16 opcode;
    MessageArgType args[MESSAGE_MAX_ARGS];
} MessageSignature;

typedef struct MessageHeader {
    u32 obj_id;
    u16 opcode;
    u16 length;
} MessageHeader;

typedef struct Message {
    MessageHeader head;
    u8* buf;
} Message;

Message* message_new(u32 obj_id, MessageSignature signature, ...);

void message_destroy(Message* message);
