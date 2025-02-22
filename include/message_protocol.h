#pragma once

#include "dwl/protocol.h"
#include "message.h"

#define DISPLAY_SYNC_SIGNATURE "n"
#define DISPLAY_GET_REGISTRY_SIGNATURE "n"
#define DISPLAY_ERROR_SIGNATURE "ous"
#define DISPLAY_DELETE_ID_SIGNATURE "u"

static inline Message* display_sync_message(ID callback) {
    return message_new(DWL_DISPLAY_OBJECT_ID, DWL_DISPLAY_REQUEST_SYNC, DISPLAY_SYNC_SIGNATURE, callback);
}

static inline Message* display_get_registry_message(ID registry) {
    return message_new(DWL_DISPLAY_OBJECT_ID, DWL_DISPLAY_REQUEST_GET_REGISTRY, DISPLAY_GET_REGISTRY_SIGNATURE, registry);
}
