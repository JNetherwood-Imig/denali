#include "dwl/defines.h"
#include "message.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

Message* message_new(u32 obj_id, u16 opcode, const char* const signature, ...) {
    Message* message = malloc(sizeof(Message));
    message->head = (MessageHeader){
        .length = sizeof(MessageHeader),
        .obj_id = obj_id,
        .opcode = opcode,
    };

    usize arg_count = strlen(signature);

    va_list argp;

    va_start(argp);    
    for (usize i = 0; i < arg_count; i++) {
        switch (signature[i]) {
        case MESSAGE_ARG_TYPE_INT:
        case MESSAGE_ARG_TYPE_UINT:
        case MESSAGE_ARG_TYPE_FIXED:
        case MESSAGE_ARG_TYPE_OBJECT:
        case MESSAGE_ARG_TYPE_NEW_ID:
        case MESSAGE_ARG_TYPE_ENUM:
            va_arg(argp, u32);
            message->head.length += sizeof(u32);
            break;
        case MESSAGE_ARG_TYPE_STRING:
            message->head.length += sizeof(u32);
            const char* const str = va_arg(argp, const char* const);
            message->head.length += ROUNDUP_4(strlen(str) + 1);
            break;
        case MESSAGE_ARG_TYPE_ARRAY:
            message->head.length += ROUNDUP_4(va_arg(argp, u32));
            va_arg(argp, const void* const);
            break;
        case MESSAGE_ARG_TYPE_FD:
            va_arg(argp, i32);
            break;
        }
    }
    va_end(argp);

    message->buf = malloc(message->head.length);
    memset(message->buf, 0, message->head.length);
    memcpy(message->buf, &message->head, sizeof(MessageHeader));
    message->ptr = message->buf + sizeof(MessageHeader);

    va_start(argp);
    for (usize i = 0; i < arg_count; i++) {
        switch (signature[i]) {
        case MESSAGE_ARG_TYPE_INT:
        case MESSAGE_ARG_TYPE_UINT:
        case MESSAGE_ARG_TYPE_FIXED:
        case MESSAGE_ARG_TYPE_OBJECT:
        case MESSAGE_ARG_TYPE_NEW_ID:
        case MESSAGE_ARG_TYPE_ENUM:
            const u32 val = va_arg(argp, const u32);
            memcpy(message->ptr, &val, sizeof(val));
            message->ptr += sizeof(val);
            break;
        case MESSAGE_ARG_TYPE_STRING:
            const char* const str = va_arg(argp, const char* const);
            const u32 len = strlen(str) + 1;
            memcpy(message->ptr, str, sizeof(len));
            message->ptr += ROUNDUP_4(len);
            break;
        case MESSAGE_ARG_TYPE_ARRAY:
            const u32 arr_len = va_arg(argp, const u32);
            memcpy(message->ptr, &arr_len, sizeof(arr_len));
            message->ptr += sizeof(arr_len);
            const void* const arr = va_arg(argp, const void* const);
            memcpy(message->ptr, arr, arr_len);
            message->ptr += ROUNDUP_4(arr_len);
            break;
        case MESSAGE_ARG_TYPE_FD:
            /*
            TODO: Handle FDs
            wayland-book.com
            "0-bit value on the primary transport, but transfers a file descriptor to the other end using the ancillary data in the Unix domain socket message (msg_control)."

            For now just increment the arg pointer
            This will be needed for wl_shm::create_pool, wl_data_offer::recieve, wl_data_source::send, and wl_keyboard::keymap
            */
            va_arg(argp, i32);
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
