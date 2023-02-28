#pragma once

#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include <stdint.h>

#define TFT_BL 2
#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

bool init_screen(bool small_screen);
