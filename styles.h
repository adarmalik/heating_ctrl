#pragma once

#include <lvgl.h>

// Should we use themes?
class Styles {
private:
  Styles(){
    lv_style_init(&RoomWidgetBaseStyle);

    lv_style_set_shadow_width(&RoomWidgetBaseStyle, 5);
    lv_style_set_shadow_spread(&RoomWidgetBaseStyle, 5);
    lv_style_set_shadow_color(&RoomWidgetBaseStyle, lv_palette_darken(LV_PALETTE_GREY, 4));
    lv_style_set_text_font(&RoomWidgetBaseStyle, &lv_font_montserrat_28);
    lv_style_set_bg_color(&RoomWidgetBaseStyle, lv_palette_lighten(LV_PALETTE_GREY, 1));
    lv_style_set_bg_grad_color(&RoomWidgetBaseStyle, lv_palette_darken(LV_PALETTE_GREY, 2));
    lv_style_set_bg_grad_dir(&RoomWidgetBaseStyle, LV_GRAD_DIR_VER);
    lv_style_set_text_color(&RoomWidgetBaseStyle, lv_color_white());

    lv_style_init(&RoomWidgetNameStyle);
    lv_style_set_text_font(&RoomWidgetNameStyle, &lv_font_montserrat_24);

    lv_style_init(&RoomWidgetTempStyle);
    lv_style_set_text_font(&RoomWidgetTempStyle, &lv_font_montserrat_16);

    lv_style_init(&RoomWidgetSetPointStyle);
    lv_style_set_text_font(&RoomWidgetSetPointStyle, &lv_font_montserrat_16);

    lv_style_init(&RoomNameStyle);
    lv_style_set_text_font(&RoomNameStyle, &lv_font_montserrat_44);

    lv_style_init(&DialogStyle);
    lv_style_set_bg_color(&DialogStyle, lv_palette_lighten(LV_PALETTE_GREY, 1));

    lv_style_init(&DialogButtonStyle);
    lv_style_set_bg_color(&DialogButtonStyle, lv_palette_darken(LV_PALETTE_GREY, 2));
    lv_style_set_border_color(&DialogButtonStyle, lv_palette_darken(LV_PALETTE_GREY, 4));
    lv_style_set_border_width(&DialogButtonStyle, 3);

    lv_style_init(&DialogSliderStyle);
    lv_style_set_bg_color(&DialogSliderStyle, lv_palette_darken(LV_PALETTE_GREY, 2));
    lv_style_set_border_color(&DialogSliderStyle, lv_palette_darken(LV_PALETTE_GREY, 4));
    lv_style_set_border_width(&DialogSliderStyle, 3);
    lv_style_set_bg_grad_color(&DialogSliderStyle,lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_grad_dir(&DialogSliderStyle,LV_GRAD_DIR_HOR);
    lv_style_set_shadow_color(&DialogSliderStyle,lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_width(&DialogSliderStyle, 10);
    lv_style_set_shadow_spread(&DialogSliderStyle, 1);
    lv_style_set_text_font(&DialogSliderStyle, &lv_font_montserrat_30);

    lv_style_init(&DialogSliderActiveStyle);
    lv_style_set_bg_color(&DialogSliderActiveStyle, lv_palette_darken(LV_PALETTE_GREY, 2));
    lv_style_set_border_color(&DialogSliderActiveStyle, lv_palette_darken(LV_PALETTE_GREY, 4));
    lv_style_set_border_width(&DialogSliderActiveStyle, 3);
    lv_style_set_bg_color(&DialogSliderActiveStyle, lv_palette_darken(LV_PALETTE_GREY, 3));
    lv_style_set_text_font(&DialogSliderActiveStyle, &lv_font_montserrat_44);

    lv_style_init(&MainButtonStyle);
    lv_style_set_bg_color(&MainButtonStyle, lv_palette_darken(LV_PALETTE_GREY, 2));
    lv_style_set_border_color(&MainButtonStyle, lv_palette_darken(LV_PALETTE_GREY, 1));
    lv_style_set_border_width(&MainButtonStyle, 3);
    lv_style_set_bg_color(&MainButtonStyle, lv_palette_lighten(LV_PALETTE_GREY, 3));
    lv_style_set_bg_grad_color(&MainButtonStyle, lv_palette_darken(LV_PALETTE_GREY, 4));
    lv_style_set_bg_grad_dir(&MainButtonStyle, LV_GRAD_DIR_VER);
    lv_style_set_text_font(&MainButtonStyle, &lv_font_montserrat_20);
  };

public:
  static Styles &getInstance() {
    static Styles instance;
    return instance;
  };
  Styles(Styles const &) = delete;
  void operator=(Styles const &) = delete;

  lv_style_t RoomWidgetBaseStyle;
  lv_style_t RoomWidgetNameStyle;
  lv_style_t RoomWidgetTempStyle;
  lv_style_t RoomWidgetSetPointStyle;
  lv_style_t RoomNameStyle;
  lv_style_t DialogStyle;
  lv_style_t DialogButtonStyle;
  lv_style_t MainButtonStyle;
  lv_style_t DialogSliderStyle;
  lv_style_t DialogSliderActiveStyle;
  lv_style_t LedStyle;
};
