#[repr(C)]
pub struct DwlDisplay {
    sockfd: i32
}

#[link(name = "denali")]
extern "C" {
    pub fn dwl_display_disconnect(display: *mut DwlDisplay);
}