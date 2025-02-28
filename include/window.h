#pragma once

#include "dwl/window.h"
#include "display.h"

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
