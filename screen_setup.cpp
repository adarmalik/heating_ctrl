#pragma once

#include <lvgl.h>
#include <Arduino_GFX_Library.h>

#include <TAMC_GT911.h>
#include <Wire.h>

/* Change to your screen resolution */
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
    GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */
);

// ST7262 IPS LCD 800x480
 Arduino_RPi_DPI_RGBPanel *gfx = new Arduino_RPi_DPI_RGBPanel(
   bus,
   800 /* width */, 0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
   480 /* height */, 0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
   1 /* pclk_active_neg */, 14000000 /* prefer_speed */, true /* auto_flush */);

#define TFT_BL 2
#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin


#define TOUCH_GT911
#define TOUCH_GT911_SCL 20
#define TOUCH_GT911_SDA 19
#define TOUCH_GT911_INT -1
#define TOUCH_GT911_RST 38
#define TOUCH_GT911_ROTATION ROTATION_NORMAL
//#define TOUCH_MAP_X1 800
//#define TOUCH_MAP_X1 480
#define TOUCH_MAP_X2 0
//#define TOUCH_MAP_Y1 480
//#define TOUCH_MAP_Y1 272
#define TOUCH_MAP_Y2 0


TAMC_GT911 *ts = NULL;

uint16_t touch_last_x = 0;
uint16_t touch_last_y = 0;

int touch_map_x1 = 0;
int touch_map_y1 = 0;

bool touch_has_signal() { return true; }
bool touch_released() { return true; }

void touch_init(bool small) {
  touch_map_x1 = small ? 480 : 800;
  touch_map_y1 = small ? 272 : 480;


  ts = new TAMC_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT,
                           TOUCH_GT911_RST, max(touch_map_x1, TOUCH_MAP_X2),
                           max(TOUCH_MAP_Y2, touch_map_y1));
  Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
  ts->begin();
  ts->setRotation(TOUCH_GT911_ROTATION);
}

bool touch_touched() {
  ts->read();
  if (ts->isTouched) {
#if defined(TOUCH_SWAP_XY)
    touch_last_x =
        map(ts->points[0].y, touch_map_x1, TOUCH_MAP_X2, 0, gfx->width() - 1);
    touch_last_y =
        map(ts->points[0].x, touch_map_y1, TOUCH_MAP_Y2, 0, gfx->height() - 1);
#else
    touch_last_x =
        map(ts->points[0].x, touch_map_x1, TOUCH_MAP_X2, 0, gfx->width() - 1);
    touch_last_y =
        map(ts->points[0].y, touch_map_y1, TOUCH_MAP_Y2, 0, gfx->height() - 1);
#endif
    return true;
  } else {
    return false;
  }
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  if (touch_has_signal()) {
    if (touch_touched()) {
      data->state = LV_INDEV_STATE_PR;
      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
    } else if (touch_released()) {
      data->state = LV_INDEV_STATE_REL;
    }
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

bool init_screen(bool small_screen){
  bool rc = false;

  // Init Display
  gfx->begin();

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  lv_init();
  delay(10);
  touch_init(small_screen);
  screenWidth = gfx->width();
  screenHeight = gfx->height();

  disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * screenHeight/4 , MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

  if (!disp_draw_buf){
    Serial.println("LVGL disp_draw_buf allocate failed!");
  } else {
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * screenHeight/4);

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    Serial.println("Screen setup done");
    rc = true;
  }
  return rc;
}
