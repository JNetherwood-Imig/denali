#pragma once

#include "display.h"
#include "dwl/window_core.h"

typedef Object WlSurface;
typedef Object XdgSurface;
typedef Object XdgToplevel;
typedef Object XdgPopup;

typedef struct DwlWindow {
    DwlDisplay* display;

    WlSurface wl_surface;
    XdgSurface xdg_surface;

    DwlWindowType type;
    union {
        XdgToplevel toplevel;
        XdgPopup popup;
    };
} DwlWindow;
