#include "display_priv.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

void dwl_display_disconnect(DwlDisplay* display);

DwlDisplay* dwl_display_connect(DwlDisplayError* err) {
    if (err) {
        *err = DWL_DISPLAY_ERROR_NONE;
    }
    DwlDisplay* display = malloc(sizeof(*display));

    const char* wayland_sock = getenv("WAYLAND_SOCK");
    if (wayland_sock) {
        display->sockfd = strtol(wayland_sock, nullptr, 10);
        return display;
    }

    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
    };

    const char* xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
    if (!xdg_runtime_dir) {
        if (err) {
            *err = DWL_DISPLAY_ERROR_NO_XDG_RUNTIME_DIR;
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

    display->sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (display->sockfd < 0) {
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
        goto err;
    }

    if (connect(display->sockfd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
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
        goto err;
    }

    return display;

err:
    dwl_display_disconnect(display);
    return nullptr;
}

void dwl_display_disconnect(DwlDisplay* display) {
    if (!display)
        return;

    if (display->sockfd > 0)
        close(display->sockfd);

    free(display);
}
