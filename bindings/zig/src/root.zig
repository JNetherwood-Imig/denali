const c = @cImport(@cInclude("display.h"));

pub const Display = opaque {
    pub const Error = error{
        no_xdg_runtime_dir,
        socket_permission_denied,
        socket_fd_limit_reached,
        socket_no_mem,
        connect_permission_denied,
        connect_would_block,
        connect_refused,
        unknown,
    };

    pub inline fn connect() Error!*Display {
        var err: c.DisplayError = c.DISPLAY_ERROR_NONE;
        const disp = c.display_connect(&err);
        return @ptrCast(disp orelse return switch (err) {
            c.DISPLAY_ERROR_NO_XDG_RUNTIME_DIR => Error.no_xdg_runtime_dir,
            c.DISPLAY_ERROR_SOCKET_PERMISSION_DENIED => Error.socket_permission_denied,
            c.DISPLAY_ERROR_SOCKET_FD_LIMIT_REACHED => Error.socket_fd_limit_reached,
            c.DISPLAY_ERROR_SOCKET_NO_MEM => Error.socket_no_mem,
            c.DISPLAY_ERROR_CONNECT_PERMISSION_DENIED => Error.connect_permission_denied,
            c.DISPLAY_ERROR_CONNECT_WOULD_BLOCK => Error.connect_would_block,
            c.DISPLAY_ERROR_CONNECT_REFUSED => Error.connect_refused,
            else => Error.unknown,
        });
    }

    pub inline fn destroy(self: *Display) void {
        c.display_disconnect(self);
    }
};
