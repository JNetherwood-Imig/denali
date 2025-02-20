fn main() {
    let display = dwl_display_connect(nullptr);
    if (!display) {
      printf("Error connecting to display!\n");
      return -1;  
    }
    
    printf("Connected!\n");
  
    dwl_display_disconnect(display);
  }
  