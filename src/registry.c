#include "display_priv.h"
#include "protocol.h"
#include "vector.h"

#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>

#define REGISTRY_INITIAL_CAPACITY 32U

bool display_get_registry(DwlDisplay* display) {
    display->registry.id = display->next_id++;
    display->registry.display = display;
    display->registry.globals = nullptr;
    vector_reserve(display->registry.globals, REGISTRY_INITIAL_CAPACITY);

    display_send_message(display, display->id, WL_DISPLAY_REQUEST_GET_REGISTRY,
                         WL_DISPLAY_GET_REGISTRY_SIGNATURE, display->registry.id);

    struct epoll_event event;
    epoll_wait(display->epoll_fd, &event, 1, -1);
    while (true) {
        if (event.data.fd == display->sockfd) {
            Message m = display_read_message(display);
            const u32 name = message_read_u32(&m);
            const char* const interface = message_read_str(&m);
            const u32 version = message_read_u32(&m);

            RegistryGlobal glob = {.name = name, .version = version};
            strncpy(glob.interface, interface, sizeof(glob.interface));

            vector_push_back(display->registry.globals, glob);
            printf("pushing to vector, length is now %zu\n",
                   vector_size(display->registry.globals));

            message_destroy(&m);
        }
        if ((epoll_wait(display->epoll_fd, &event, 1, 0)) == 0)
            break;
    }

    return true;
}

void display_destroy_registry(DwlDisplay* display) {
    vector_destroy(display->registry.globals);
}

Object registry_get_object(Registry* registry, RegistryGlobal* global, u32 version) {
    Object obj = {
        .id = registry->display->next_id++,
        .bound_version = version,
        .global = *global,
        .display = registry->display,
    };

    display_send_message(registry->display, registry->id, WL_REGISTRY_REQUEST_BIND,
                         WL_REGISTRY_BIND_SIGNATURE, global->name, global->interface,
                         version, obj.id);
    return obj;
}

RegistryGlobal* registry_lookup_interface(Registry* registry,
                                          const char* const interface) {
    for (usize i = 0; i < vector_size(registry->globals); ++i) {
        if (strcmp(registry->globals[i].interface, interface) == 0)
            return &registry->globals[i];
    }

    return nullptr;
}
