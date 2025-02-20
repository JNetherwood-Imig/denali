#include "display.h"

#include <stdio.h>

int main(void) {
  Display* display = display_connect(nullptr);
  if (!display) {
    printf("Error connecting to display!\n");
    return -1;  
  }
  
  printf("Connected!\n");

  display_disconnect(display);
}
