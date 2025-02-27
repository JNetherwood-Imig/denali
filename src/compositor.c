#include "display.h"
#include "protocol.h"
#include "window.h"

#include <assert.h>

bool display_get_compositor(DwlDisplay* display) {
    RegistryGlobal* global =
        registry_lookup_interface(&display->registry, "wl_compositor");
    assert(global != nullptr && "Failed to get wl_compositor global");
    // TODO: Don't just bind to implemented version without changing behavior
    // accordingly
    registry_init_object(&display->compositor, &display->registry, global,
                         global->version);

    return true;
}

WlSurface compositor_create_surface(WlCompositor* compositor) {
    WlSurface surface = {
        .id = display_make_new_id(compositor->display),
    };

    display_send_message(compositor->display, compositor->id,
                         WL_COMPOSITOR_REQUEST_CREATE_SURFACE,
                         WL_COMPOSITOR_CREATE_SURFACE_SIGNATURE, surface.id);

    return surface;
}
