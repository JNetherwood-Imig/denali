#include "defines_priv.h"
#include "display_priv.h"
#include "message.h"

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static i32 create_socket(DwlDisplayError* err) {
    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
    };

    const char* xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
    if (!xdg_runtime_dir) {
        if (err) {
            *err = DWL_DISPLAY_ERROR_NO_XDG_RUNTIME_DIR;
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
                *err = DWL_DISPLAY_ERROR_SOCKET_PERMISSION_DENIED;
                break;
            case ENFILE:
            case EMFILE:
                *err = DWL_DISPLAY_ERROR_SOCKET_FD_LIMIT_REACHED;
                break;
            case ENOBUFS:
            case ENOMEM:
                *err = DWL_DISPLAY_ERROR_SOCKET_NO_MEM;
                break;
            default:
                *err = DWL_DISPLAY_ERROR_UNKNOWN;
                break;
            }
        }
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        if (err) {
            switch (errno) {
            case EACCES:
                *err = DWL_DISPLAY_ERROR_CONNECT_PERMISSION_DENIED;
                break;
            case EAGAIN:
                *err = DWL_DISPLAY_ERROR_CONNECT_WOULD_BLOCK;
                break;
            case ECONNREFUSED:
                *err = DWL_DISPLAY_ERROR_CONNECT_REFUSED;
                break;
            default:
                *err = DWL_DISPLAY_ERROR_UNKNOWN;
                break;
            }
        }
        close(sockfd);
        return -1;
    }

    return sockfd;
}

static i32 setup_epoll(const i32 display_fd, DwlDisplayError* err) {
    i32 fd = epoll_create1(0);
    if (fd < 0) {
        if (err) {
            switch (errno) {
            case EMFILE:
            case ENFILE:
                *err = DWL_DISPLAY_ERROR_EPOLL_FD_LIMIT_REACHED;
                break;
            case ENOMEM:
                *err = DWL_DISPLAY_ERROR_EPOLL_NO_MEM;
                break;
            default:
                *err = DWL_DISPLAY_ERROR_UNKNOWN;
                break;
            }
        }
        return -1;
    }

    if (epoll_ctl(fd, EPOLL_CTL_ADD, display_fd,
                  &(struct epoll_event){.events = EPOLLIN, .data.fd = display_fd}) !=
        0) {
        if (err) {
            switch (errno) {
            case ENOMEM:
                *err = DWL_DISPLAY_ERROR_EPOLL_NO_MEM;
                break;
            case EPERM:
                *err = DWL_DISPLAY_ERROR_EPOLL_PERMISSION_DENIED;
                break;
            default:
                *err = DWL_DISPLAY_ERROR_UNKNOWN;
                break;
            }

            close(fd);
            return -1;
        }
    }

    return fd;
}

void display_send_message(DwlDisplay* display, ID id, u16 opcode,
                          const char* signature, ...) {
    Message message = {};
    message.head = (MessageHeader){
        .length = sizeof(MessageHeader),
        .obj_id = id,
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
            message.head.length += sizeof(u32);
            break;
        case MESSAGE_ARG_TYPE_STRING:
            message.head.length += sizeof(u32);
            const char* const str = va_arg(argp, const char* const);
            message.head.length += roundup_4(strlen(str) + 1);
            break;
        case MESSAGE_ARG_TYPE_ARRAY:
            message.head.length += roundup_4(va_arg(argp, u32));
            va_arg(argp, const void* const);
            break;
        case MESSAGE_ARG_TYPE_FD:
            va_arg(argp, i32);
            break;
        }
    }
    va_end(argp);

    message.buf = malloc(message.head.length);
    memset(message.buf, 0, message.head.length);
    memcpy(message.buf, &message.head, sizeof(MessageHeader));
    message.ptr = message.buf + sizeof(MessageHeader);

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
            memcpy(message.ptr, &val, sizeof(val));
            message.ptr += sizeof(val);
            break;
        case MESSAGE_ARG_TYPE_STRING:
            const char* const str = va_arg(argp, const char* const);
            const u32 len = strlen(str) + 1;
            memcpy(message.ptr, str, sizeof(len));
            message.ptr += roundup_4(len);
            break;
        case MESSAGE_ARG_TYPE_ARRAY:
            const u32 arr_len = va_arg(argp, const u32);
            memcpy(message.ptr, &arr_len, sizeof(arr_len));
            message.ptr += sizeof(arr_len);
            const void* const arr = va_arg(argp, const void* const);
            memcpy(message.ptr, arr, arr_len);
            message.ptr += roundup_4(arr_len);
            break;
        case MESSAGE_ARG_TYPE_FD:
            /*
            TODO: Handle FDs
            wayland-book.com
            "0-bit value on the primary transport, but transfers a file
            descriptor to the other end using the ancillary data in the Unix
            domain socket message (msg_control)."

            For now just increment the arg pointer
            This will be needed for wl_shm::create_pool, wl_data_offer::recieve,
            wl_data_source::send, and wl_keyboard::keymap
            */
            va_arg(argp, i32);
            break;
        }
    }
    va_end(argp);

    const usize bytes_written =
        write(display->sockfd, message.buf, message.head.length);
    assert(bytes_written == message.head.length);

    message_destroy(&message);
}

Message display_read_message(DwlDisplay* display) {
    MessageHeader h;
    usize bytes_read = read(display->sockfd, &h, sizeof(h));
    assert(bytes_read == sizeof(h));
    Message m = {.head = h};
    m.buf = malloc(h.length - sizeof(h));
    m.ptr = m.buf;
    bytes_read = read(display->sockfd, m.buf, h.length - sizeof(h));
    assert(bytes_read == h.length - sizeof(h));
    return m;
}

ID display_make_new_id(DwlDisplay* display) { return display->next_id++; }

DwlDisplay* dwl_display_connect(DwlDisplayError* err) {
    DwlDisplay* display = malloc(sizeof(*display));
    display->id = WL_DISPLAY_OBJECT_ID;
    display->next_id = WL_DISPLAY_OBJECT_ID + 1;

    const char* wayland_sock = getenv("WAYLAND_SOCK");
    char* endp = nullptr;
    display->sockfd =
        wayland_sock ? strtol(wayland_sock, &endp, 10) : create_socket(err);
    if (display->sockfd < 0 || endp != 0)
        goto err_sock;

    display->epoll_fd = setup_epoll(display->sockfd, err);
    if (display->epoll_fd < 0)
        goto err_epoll;

    if (!display_get_registry(display))
        goto err_registry;

    if (!display_get_compositor(display))
        goto err_compositor;

    if (!display_get_shell(display))
        goto err_compositor;

    return display;

err_compositor:
    display_destroy_registry(display);
err_registry:
    close(display->epoll_fd);
err_epoll:
    close(display->sockfd);
err_sock:
    free(display);
    return nullptr;
}

void dwl_display_disconnect(DwlDisplay* display) {
    display_destroy_registry(display);
    if (display->epoll_fd > 0)
        close(display->epoll_fd);

    if (display->sockfd > 0)
        close(display->sockfd);

    free(display);
}
