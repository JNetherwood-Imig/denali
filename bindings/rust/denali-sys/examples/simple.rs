fn main() {
    unsafe {
        let display = denali_sys::dwl_display_connect(std::ptr::null_mut());
        if display.is_null() {
            eprintln!("Failed to connect to display");
            return;
        }
        println!("Connected!");

        denali_sys::dwl_display_disconnect(display);
    }
}
