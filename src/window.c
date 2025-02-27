#include "window.h"
#include "dwl/defines.h"
#include "list.h"

#include <stdlib.h>

typedef struct Test {
    i32 something;
    ListNode link;
} Test;

DwlWindow* dlw_window_create(DwlDisplay* display, DwlWindowType type,
                             DwlWindowError* err) {
    (void)display;
    (void)type;
    (void)err;
    DwlWindow* window = malloc(sizeof(DwlWindow));
    return window;
}

void dwl_window_destroy(DwlWindow* window) { free(window); }
