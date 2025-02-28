#pragma once

#include "dwl/display.h"
#include "registry.h"
#include "message.h"
#include "protocol.h"

struct DwlDisplay {
    ID id;
    ID next_id;

    Registry registry;

    i32 sockfd;
    i32 epoll_fd;

    u8* buf;
    u8* buf_ptr;
    usize buf_capacity;
};

void display_send_message(DwlDisplay* display, ID obj, u16 opcode,
                          const char* signature, ...);

bool display_read_message(DwlDisplay* display, Message* m);

ID display_make_new_id(DwlDisplay* display);

bool display_get_registry(DwlDisplay* display);

void display_destroy_registry(DwlDisplay* display);
