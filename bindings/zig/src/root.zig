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
        Unknown,
    };

    pub inline fn connect() Error!*Display {
        var err: c.DwlDisplayError = undefined;
        const disp = c.dwl_display_connect(&err);
        return @ptrCast(disp orelse return switch (err) {
            c.DWL_DISPLAY_ERROR_NO_XDG_RUNTIME_DIR => error.NoXdgRuntimeDir,
            c.DWL_DISPLAY_ERROR_SOCKET_PERMISSION_DENIED => error.SocketPermissionDenied,
            c.DWL_DISPLAY_ERROR_SOCKET_FD_LIMIT_REACHED => error.SocketFdLimitReached,
            c.DWL_DISPLAY_ERROR_SOCKET_NO_MEM => error.SocketNoMem,
            c.DWL_DISPLAY_ERROR_CONNECT_PERMISSION_DENIED => error.ConnectPermissionDenied,
            c.DWL_DISPLAY_ERROR_CONNECT_WOULD_BLOCK => error.ConnectWouldBlock,
            c.DWL_DISPLAY_ERROR_CONNECT_REFUSED => error.ConnectRefused,
            else => error.Unknown,
        });
    }

    pub inline fn destroy(self: *Display) void {
        c.dwl_display_disconnect(self);
    }
};
