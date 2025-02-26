#include "message.h"
#include "dwl/defines.h"

#include <stdarg.h>
#include <stdlib.h>

i32 message_read_i32(Message* m) {
    const i32 val = *(i32*)m->ptr;
    m->ptr += sizeof(i32);
    return val;
}

u32 message_read_u32(Message* m) {
    const u32 val = *(u32*)m->ptr;
    m->ptr += sizeof(u32);
    return val;
}

const char* message_read_str(Message* m) {
    const u32 len = message_read_u32(m);
    const char* str = (char*)m->ptr;
    m->ptr += roundup_4(len);
    return str;
}

void message_destroy(Message* m) { free(m->buf); }
