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

typedef struct DwlDisplay DwlDisplay;
typedef struct Registry {
    ID id;
    Vector(RegistryGlobal) globals;
    DwlDisplay* display;
} Registry;

typedef struct Object {
    ID id;
    RegistryGlobal global;
    u32 bound_version;
    DwlDisplay* display;
} Object;

typedef Object WlCompositor;
typedef Object XdgWmBase;

struct DwlDisplay {
    ID id;
    ID next_id;

    Registry registry;

    WlCompositor compositor;
    XdgWmBase shell;
    // seat
    // keyboard
    // pointer
    // touch
    // shm
    // outputs (layoyt?)

    i32 sockfd;
    i32 epoll_fd;

    u8* buf;
    u8* buf_ptr;
    usize buf_capacity;
};

void display_send_message(DwlDisplay* display, ID obj, u16 opcode,
                          const char* signature, ...);

Message display_read_message(DwlDisplay* display);

ID display_make_new_id(DwlDisplay* display);

bool display_get_registry(DwlDisplay* display);

bool display_get_compositor(DwlDisplay* display);

bool display_get_shell(DwlDisplay* display);

void display_destroy_registry(DwlDisplay* display);

Object* registry_create_object(Registry* registry, RegistryGlobal* global,
                               u32 version);

void registry_init_object(Object* object, Registry* registry, RegistryGlobal* global,
                          u32 version);

RegistryGlobal* registry_lookup_interface(Registry* registry,
                                          const char* const interface);
