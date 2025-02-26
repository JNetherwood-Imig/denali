#include "dwl/defines.h"
#include "list.h"
#include "window_priv.h"

#include <stdlib.h>

typedef struct Test {
    i32 something;
    ListNode link;
} Test;

DwlWindow* dlw_window_create(DwlDisplay* display, DwlWindowType type,
                             DwlWindowError* err) {
    DwlWindow* window = malloc(sizeof(DwlWindow));
    return window;
}

void dwl_window_destroy(DwlWindow* window) { free(window); }
