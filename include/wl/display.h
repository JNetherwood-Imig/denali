#pragma once

#include "display_common.h"

typedef void Display;

Display* display_connect(DisplayError* out_error);

void display_disconnect(Display* display);
