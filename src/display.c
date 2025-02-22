#include "dwl/defines.h"
#include "dwl/display_core.h"
#include "dwl/protocol.h"
#include "message.h"
#include "message_protocol.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

typedef Object DwlRegistry;

typedef struct DwlDisplay {
    Object parent;

    DwlRegistry registry;

    u32 next_id;

    i32 sockfd;
    i32 epoll_fd;
} DwlDisplay;

static i32 create_socket(DwlDisplayResult* err) {
    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
    };

    const char* xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
    if (!xdg_runtime_dir) {
        if (err) {
            *err = DWL_DISPLAY_RESULT_NO_XDG_RUNTIME_DIR;
        }
        return -1;
    }

    const char* wayland_display = getenv("WAYLAND_DISPLAY");
    if (!wayland_display)
        wayland_display = "wayland-0";

    if (wayland_display[0] == '/')
        snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", wayland_display);
    else
        snprintf(addr.sun_path, sizeof(addr.sun_path), "%s/%s", xdg_runtime_dir,
                 wayland_display);

    i32 sockfd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sockfd < 0) {
        if (err) {
            switch (errno) {
            case EACCES:
                *err = DWL_DISPLAY_RESULT_SOCKET_PERMISSION_DENIED;
                break;
            case ENFILE:
            case EMFILE:
                *err = DWL_DISPLAY_RESULT_SOCKET_FD_LIMIT_REACHED;
                break;
            case ENOBUFS:
            case ENOMEM:
                *err = DWL_DISPLAY_RESULT_SOCKET_NO_MEM;
                break;
            default:
                *err = DWL_DISPLAY_RESULT_UNKNOWN;
                break;
            }
        }
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        if (err) {
            switch (errno) {
            case EACCES:
                *err = DWL_DISPLAY_RESULT_CONNECT_PERMISSION_DENIED;
                break;
            case EAGAIN:
                *err = DWL_DISPLAY_RESULT_CONNECT_WOULD_BLOCK;
                break;
            case ECONNREFUSED:
                *err = DWL_DISPLAY_RESULT_CONNECT_REFUSED;
                break;
            default:
                *err = DWL_DISPLAY_RESULT_UNKNOWN;
                break;
            }
        }
        close(sockfd);
        return -1;
    }

    return sockfd;
}

static i32 setup_epoll(const i32 display_fd, DwlDisplayResult* err) {
    i32 fd = epoll_create1(0);
    if (fd < 0) {
        if (err) {
            switch (errno) {
            case EMFILE:
            case ENFILE:
                *err = DWL_DISPLAY_RESULT_EPOLL_FD_LIMIT_REACHED;
                break;
            case ENOMEM:
                *err = DWL_DISPLAY_RESULT_EPOLL_NO_MEM;
                break;
            default:
                *err = DWL_DISPLAY_RESULT_UNKNOWN;
                break;
            }
        }
        return -1;
    }

    if (epoll_ctl(
        fd, EPOLL_CTL_ADD, display_fd,
        &(struct epoll_event){.events = EPOLLIN, .data.fd = display_fd}) != 0) {
        if (err) {
            switch (errno) {
                case ENOMEM:
                    *err = DWL_DISPLAY_RESULT_EPOLL_NO_MEM;
                    break;
                case EPERM:
                    *err = DWL_DISPLAY_RESULT_EPOLL_PERMISSION_DENIED;
                    break;
                default:
                    *err = DWL_DISPLAY_RESULT_UNKNOWN;
                    break;
            }

            close(fd);
            return -1;
        }
    }

    return fd;
}

static void display_send_message(DwlDisplay* display, Message* message) {
    const usize bytes_written = write(display->sockfd, message->buf, message->head.length);
    assert(bytes_written == message->head.length);
}

static bool display_get_registry(DwlDisplay* display, DwlDisplayResult* err) {
    (void)err;
    display->registry.id = display->next_id++;
    Message* msg = display_get_registry_message(display->registry.id);
    display_send_message(display, msg);
    message_destroy(msg);

    return true;
}

DwlDisplay* dwl_display_connect(DwlDisplayResult* err) {
    DwlDisplay* display = malloc(sizeof(*display));
    display->parent.id = DWL_DISPLAY_OBJECT_ID;
    display->next_id = DWL_DISPLAY_OBJECT_ID + 1;

    const char* wayland_sock = getenv("WAYLAND_SOCK");
    char* endp = nullptr;
    display->sockfd = wayland_sock ? strtol(wayland_sock, &endp, 10) : create_socket(err);
    if (display->sockfd < 0 || endp != 0)
        goto err_sock;

    display->epoll_fd = setup_epoll(display->sockfd, err);
    if (display->epoll_fd < 0)
        goto err_epoll;

    if (!display_get_registry(display, err))
        goto err_registry;

    // REMOVE: TEMP
    {
        struct epoll_event events[32];
        epoll_wait(display->epoll_fd, events, 32, -1);
        char buf[4096];
        isize bytes_read = read(display->sockfd, buf, sizeof(buf));
        printf("Read %zi bytes from display sock\n", bytes_read);

        char* ptr = buf;
        while (ptr + 8 < buf + bytes_read) {
            const MessageHeader h = *(MessageHeader*)ptr;
            ptr += sizeof(MessageHeader);
            const u32 name = *(u32*)ptr;
            ptr += sizeof(u32);
            const u32 iface_len = *(u32*)ptr;
            ptr += sizeof(u32);
            const char* iface = ptr;
            ptr += ROUNDUP_4(iface_len);
            const u32 version = *(u32*)ptr;
            ptr += sizeof(u32);

            printf("wl_registry::global\n\tName: %u, Interface: %s (Version %u)\n", name, iface, version);
        }
    }

    return display;

err_registry:
    close(display->epoll_fd);
err_epoll:
    close(display->sockfd);
err_sock:
    free(display);
    return nullptr;
}

void dwl_display_disconnect(DwlDisplay* display) {
    if (display->epoll_fd > 0)
        close(display->epoll_fd);

    if (display->sockfd > 0)
        close(display->sockfd);

    free(display);
}
