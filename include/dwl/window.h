#pragma once

#include "dwl/display.h"
#include "dwl/window_core.h"

typedef void DwlWindow;

DwlWindow* dlw_window_create(DwlDisplay* display, DwlWindowType type,
                             DwlWindowError* err);

void dwl_window_destroy(DwlWindow* window);
