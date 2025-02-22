#pragma once

#include "dwl/protocol.h"
#include "message.h"

static const MessageSignature display_request_sync = {
    DWL_DISPLAY_REQUEST_SYNC,
    {
        MESSAGE_ARG_TYPE_OBJECT,
        MESSAGE_ARG_TYPE_MAX_ENUM,
    },
};

static const MessageSignature display_request_get_registry = {
    DWL_DISPLAY_REQUEST_GET_REGISTRY,
    {
        MESSAGE_ARG_TYPE_NEW_ID,
        MESSAGE_ARG_TYPE_MAX_ENUM,
    },
};

static const MessageSignature display_event_error = {
    DWL_DISPLAY_EVENT_ERROR,
    {
        MESSAGE_ARG_TYPE_OBJECT,
        MESSAGE_ARG_TYPE_UINT,
        MESSAGE_ARG_TYPE_STRING,
        MESSAGE_ARG_TYPE_MAX_ENUM,
    },
};

static const MessageSignature display_event_delete_id = {
    DWL_DISPLAY_EVENT_DELETE_ID,
    {
        MESSAGE_ARG_TYPE_UINT,
        MESSAGE_ARG_TYPE_MAX_ENUM,
    },
};
