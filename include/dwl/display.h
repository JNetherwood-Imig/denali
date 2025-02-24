#pragma once

#include "display_core.h"

typedef void DwlDisplay;

DwlDisplay* dwl_display_connect(DwlDisplayError* out_error);

void dwl_display_disconnect(DwlDisplay* display);
