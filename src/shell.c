#include "display_priv.h"

#include <assert.h>

bool display_get_shell(DwlDisplay* display) {
    RegistryGlobal* global =
        registry_lookup_interface(&display->registry, "xdg_wm_base");
    assert(global != nullptr && "Failed to get xdg_wm_base global");
    display->shell =
        registry_get_object(&display->registry, global, global->version);

    return true;
}
