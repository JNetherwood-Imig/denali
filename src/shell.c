#include "display.h"

#include <assert.h>

bool display_get_shell(DwlDisplay* display) {
    RegistryGlobal* global =
        registry_lookup_interface(&display->registry, "xdg_wm_base");
    assert(global != nullptr && "Failed to get xdg_wm_base global");
    registry_init_object(&display->shell, &display->registry, global, global->version);

    return true;
}
