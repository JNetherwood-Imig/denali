#include "dwl/defines.h"
#include "dwl/display_core.h"
#include "dwl/protocol.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

typedef Object Registry;

typedef struct DwlDisplay {
    Object parent;

    u32 next_id;

    i32 sockfd;
    i32 epoll_fd;

    u8* buf;
    u8* buf_ptr;
    usize buf_capacity;
} DwlDisplay;

static DwlDisplay* display_create(DwlDisplayResult* err) {
    DwlDisplay* display = malloc(sizeof(*display));
    *display = (DwlDisplay){
        .parent.id = DWL_DISPLAY_OBJECT_ID,
        .next_id = DWL_DISPLAY_OBJECT_ID + 1,
        .sockfd = -1,
    };

    // Connect to existing socket
    const char* wayland_sock = getenv("WAYLAND_SOCK");
    if (wayland_sock) {
        display->sockfd = strtol(wayland_sock, nullptr, 10);
        goto setup_epoll;
    }

    // Create socket
    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
    };

    const char* xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
    if (!xdg_runtime_dir) {
        if (err) {
            *err = DWL_DISPLAY_RESULT_NO_XDG_RUNTIME_DIR;
        }
        goto err;
    }

    const char* wayland_display = getenv("WAYLAND_DISPLAY");
    if (!wayland_display)
        wayland_display = "wayland-0";

    if (wayland_display[0] == '/')
        snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", wayland_display);
    else
        snprintf(addr.sun_path, sizeof(addr.sun_path), "%s/%s", xdg_runtime_dir,
                 wayland_display);

    display->sockfd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (display->sockfd < 0) {
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
        goto err;
    }

    if (connect(display->sockfd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
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
        goto err_sock;
    }

    // Setup epoll
setup_epoll:

    if ((display->epoll_fd = epoll_create1(0)) < 0) {
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
        goto err_sock;
    }

    epoll_ctl(
        display->epoll_fd, EPOLL_CTL_ADD, display->sockfd,
        &(struct epoll_event){.events = EPOLLIN, .data.fd = display->sockfd});

    display->buf_capacity = 4096;
    display->buf = malloc(display->buf_capacity);
    display->buf_ptr = display->buf;

    return display;

err_sock:
    close(display->sockfd);
err:
    free(display);
    return nullptr;
}

static void display_destroy(DwlDisplay* display) {
    if (display->epoll_fd > 0)
        close(display->epoll_fd);

    if (display->sockfd > 0)
        close(display->sockfd);

    free(display);
}

static bool display_get_registry(DwlDisplay* display, DwlDisplayResult* err) {
    (void)display;
    (void)err;
    return true;
}

DwlDisplay* dwl_display_connect(DwlDisplayResult* err) {
    DwlDisplay* display;

    if (!(display = display_create(err)))
        return nullptr;

    if (!display_get_registry(display, err)) {
        display_destroy(display);
        return nullptr;
    }

    return display;
}

void dwl_display_disconnect(DwlDisplay* display) {
    display_destroy(display);
}
