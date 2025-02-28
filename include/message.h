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

// typedef struct Proxy Proxy;
// typedef void (*PfnProxyHandleMessage)(Proxy* self, Message* message);

// #define PROXY_MAX_HANDLERS 16U
// struct Proxy {
//     PfnProxyHandleMessage event_handlers[PROXY_MAX_HANDLERS];
// };

// // proxy->event_handlers[message->opcode](proxy, message);

// static inline void example_handler(Proxy* self, Message* message) {
//     Registry* registry = (Registry*)self;
//     const u32 name = message_read_u32(message);
//     const char* const interface = message_read_str(message);
//     const u32 version = message_read_u32(message);

//     RegistryGlobal glob = {name, version, {}};
//     strncpy(glob.interface, interface, sizeof(glob.interface));
//     vector_push_back(registry->globals, glob);
// }
