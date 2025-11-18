#pragma once

#include <tftLogger.h>

#ifdef HAS_SCREEN
extern tft_logger tft;
#define display tft
#endif
