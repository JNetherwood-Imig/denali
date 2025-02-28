#pragma once

#include "dwl/display.h"

typedef struct DwlWindow DwlWindow;

typedef enum DwlWindowType {
    DWL_WINDOW_TYPE_TOPLEVEL,
    DWL_WINDOW_TYPE_POPUP,
} DwlWindowType;

typedef enum DwlWindowError {
    DWL_WINDOW_ERROR_UNKNOWN,
} DwlWindowError;

DwlWindow* dlw_window_create(DwlDisplay* display, DwlWindowType type,
                             DwlWindowError* err);

void dwl_window_destroy(DwlWindow* window);
