#pragma once

#include "dwl/display_core.h"
#include "message.h"
#include "protocol.h"
#include "vector.h"

// This is probably not a good way to do things,
// but it shouldn't ever be problematic,
// since the longest interface I could find was 47 characters,
// 48 including '\0'
#define INTERFACE_MAX_LEN 64U

typedef struct RegistryGlobal {
    u32 name;
    u32 version;
    char interface[INTERFACE_MAX_LEN];
} RegistryGlobal;

struct DwlDisplay;
typedef struct Registry {
    ID id;
    vector(RegistryGlobal) globals;
    struct DwlDisplay* display;
} Registry;

typedef struct Object {
    ID id;
    RegistryGlobal global;
    u32 bound_version;
    struct DwlDisplay* display;
} Object;

typedef Object WlCompositor;
typedef Object XdgWmBase;

typedef struct DwlDisplay {
    ID id;
    ID next_id;

    Registry registry;

    WlCompositor compositor;
    XdgWmBase shell;

    i32 sockfd;
    i32 epoll_fd;

    u8* buf;
    u8* buf_ptr;
    usize buf_capacity;
} DwlDisplay;

void display_send_message(DwlDisplay* display, ID obj, u16 opcode,
                          const char* signature, ...);

Message display_read_message(DwlDisplay* display);

ID display_make_new_id(DwlDisplay* display);

bool display_get_registry(DwlDisplay* display);

void display_destroy_registry(DwlDisplay* display);

Object registry_get_object(Registry* registry, RegistryGlobal* global, u32 version);

RegistryGlobal* registry_lookup_interface(Registry* registry,
                                          const char* const interface);

bool display_get_compositor(DwlDisplay* display);

bool display_get_shell(DwlDisplay* display);
