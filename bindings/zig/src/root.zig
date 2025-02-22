const c = @cImport({
    @cInclude("dwl/display.h");
});

pub const Display = opaque {
    pub const Error = error{
        NoXdgRuntimeDir,
        SocketPermissionDenied,
        SocketFdLimitReached,
        SocketNoMem,
        ConnectPermissionDenied,
        ConnectWouldBlock,
        ConnectRefused,
        EpollFdLimitReached,
        EpollNoMem,
        Unknown,
    };

    pub inline fn connect() Error!*Display {
        var err: c.DwlDisplayResult = undefined;
        const disp = c.dwl_display_connect(&err);
        return @ptrCast(disp orelse return switch (err) {
            c.DWL_DISPLAY_RESULT_NO_XDG_RUNTIME_DIR => error.NoXdgRuntimeDir,
            c.DWL_DISPLAY_RESULT_SOCKET_PERMISSION_DENIED => error.SocketPermissionDenied,
            c.DWL_DISPLAY_RESULT_SOCKET_FD_LIMIT_REACHED => error.SocketFdLimitReached,
            c.DWL_DISPLAY_RESULT_SOCKET_NO_MEM => error.SocketNoMem,
            c.DWL_DISPLAY_RESULT_CONNECT_PERMISSION_DENIED => error.ConnectPermissionDenied,
            c.DWL_DISPLAY_RESULT_CONNECT_WOULD_BLOCK => error.ConnectWouldBlock,
            c.DWL_DISPLAY_RESULT_CONNECT_REFUSED => error.ConnectRefused,
            c.DWL_DISPLAY_RESULT_EPOLL_FD_LIMIT_REACHED => error.EpollFdLimitReached,
            c.DWL_DISPLAY_RESULT_EPOLL_NO_MEM => error.EpollNoMem,
            else => error.Unknown,
        });
    }

    pub inline fn destroy(self: *Display) void {
        c.dwl_display_disconnect(self);
    }
};
