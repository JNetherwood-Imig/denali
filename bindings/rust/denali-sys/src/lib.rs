#[repr(C)]
#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub struct DwlDisplay {
    sockfd: i32,
}

#[repr(transparent)]
#[derive(Debug, Copy, Clone, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct DwlDisplayError(u32);
impl DwlDisplayError {
    pub const DWL_DISPLAY_RESULT_NO_XDG_RUNTIME_DIR: Self = Self(0);
    pub const DWL_DISPLAY_RESULT_SOCKET_PERMISSION_DENIED: Self = Self(1);
    pub const DWL_DISPLAY_RESULT_SOCKET_FD_LIMIT_REACHED: Self = Self(2);
    pub const DWL_DISPLAY_RESULT_SOCKET_NO_MEM: Self = Self(3);
    pub const DWL_DISPLAY_RESULT_CONNECT_PERMISSION_DENIED: Self = Self(4);
    pub const DWL_DISPLAY_RESULT_CONNECT_WOULD_BLOCK: Self = Self(5);
    pub const DWL_DISPLAY_RESULT_CONNECT_REFUSED: Self = Self(6);
    pub const DWL_DISPLAY_RESULT_EPOLL_FD_LIMIT_REACHED: Self = Self(7);
    pub const DWL_DISPLAY_RESULT_EPOLL_NO_MEM: Self = Self(8);
    pub const DWL_DISPLAY_RESULT_UNKNOWN: Self = Self(9);
}

#[link(name = "denali")]
extern "C" {
    pub fn dwl_display_disconnect(display: *mut DwlDisplay);
    pub fn dwl_display_connect(out_error: *mut DwlDisplayError) -> *mut DwlDisplay;
}
