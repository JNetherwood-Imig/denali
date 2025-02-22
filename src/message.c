#include "message.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

Message* message_new(u32 obj_id, MessageSignature signature, ...) {
    Message* message = malloc(sizeof(Message));
    *message = (Message){
        .head =
            {
                .length = sizeof(MessageHeader),
                .obj_id = obj_id,
                .opcode = signature.opcode,
            },
        .buf = nullptr,
    };
    usize arg_count = 0;
    while (signature.args[arg_count] != MESSAGE_ARG_TYPE_MAX_ENUM)
        ++arg_count;

    va_list argp;
    va_start(argp);
    for (u32 i = 0; i < arg_count; ++i) {
        switch (signature.args[i]) {
        case MESSAGE_ARG_TYPE_NEW_ID:
        case MESSAGE_ARG_TYPE_OBJECT:
        case MESSAGE_ARG_TYPE_UINT:
            [[maybe_unused]] u32 val = va_arg(argp, u32);
            message->head.length += sizeof(u32);
            break;
        case MESSAGE_ARG_TYPE_STRING:
            const char* str = va_arg(argp, const char*);
            usize len = strlen(str);
            len = (len + 3 + sizeof(u32)) & ~3;
            message->head.length += len;
            break;
        default:
            break;
        }
    }
    va_end(argp);

    message->buf = malloc(message->head.length);
    memset(message->buf, 0, message->head.length);
    memcpy(message->buf, &message->head, sizeof(MessageHeader));
    u8* buf_ptr = message->buf + sizeof(MessageHeader);

    va_start(argp);
    for (u32 i = 0; i < arg_count; ++i) {
        switch (signature.args[i]) {
        case MESSAGE_ARG_TYPE_NEW_ID:
        case MESSAGE_ARG_TYPE_OBJECT:
        case MESSAGE_ARG_TYPE_UINT:
            u32 val = va_arg(argp, u32);
            memcpy(buf_ptr, &val, sizeof(val));
            buf_ptr += sizeof(u32);
            break;
        case MESSAGE_ARG_TYPE_STRING:
            const char* str = va_arg(argp, const char*);
            u32 len = strlen(str);
            ++len;
            memcpy(buf_ptr, &len, sizeof(u32));
            buf_ptr += sizeof(u32);
            memcpy(buf_ptr, str, len);
            buf_ptr += (len + 3) & ~3;
            break;
        default:
            break;
        }
    }
    va_end(argp);

    return message;
}

void message_destroy(Message* message) {
    free(message->buf);
    free(message);
}
