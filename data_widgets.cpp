#include "data_widgets.h"
#include "styles.h"

ActiveProfileWidget::ActiveProfileWidget(lv_obj_t *parent){
    obj = lv_label_create(parent);
    lv_obj_add_style(obj, &(Styles::getInstance().RoomWidgetTempStyle), 0);
    lv_label_set_text_fmt(obj, "-");
}

TempWidget::TempWidget(lv_obj_t *parent){
    obj = lv_label_create(parent);
    lv_obj_add_style(obj, &(Styles::getInstance().RoomWidgetTempStyle), 0);
    lv_label_set_text_fmt(obj, "%s", "- °C");
}

HumidityWidget::HumidityWidget(lv_obj_t *parent){
    obj = lv_label_create(parent);
    lv_obj_add_style(obj, &(Styles::getInstance().RoomWidgetTempStyle), 0);
    lv_label_set_text_fmt(obj, "%s", "- %");
}

ImageWidget::ImageWidget(lv_obj_t *parent, const lv_img_dsc_t *image, uint32_t color){
    activeColor = color;
    obj = lv_img_create(parent);
    lv_img_set_src(obj, image);
    lv_obj_add_style(obj, &(Styles::getInstance().RoomWidgetSetPointStyle), 0);
    lv_obj_set_style_img_recolor_opa(obj, 192, 0);
    lv_obj_set_style_img_recolor(obj, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
}

LedWidget::LedWidget(lv_obj_t* parent, String str, uint32_t color){
    obj = lv_obj_create(parent);
    activeColor = color;
    lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
    lv_obj_set_style_outline_opa(obj, LV_OPA_0, 0);
    lv_obj_set_style_border_opa(obj, LV_OPA_0, 0);
    lv_obj_set_style_shadow_opa(obj, LV_OPA_0, 0);
    lv_obj_t* lbl = lv_label_create(obj);
    lv_obj_add_style(lbl, &(Styles::getInstance().RoomWidgetSetPointStyle), 0);
    lv_label_set_text_fmt(lbl, str.c_str());
    lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 0, 0);
    stateLed = lv_led_create(obj);
    lv_obj_set_size(stateLed, 15, 15);
    lv_obj_align(stateLed, LV_ALIGN_RIGHT_MID, -20, 0);
    lv_led_set_brightness(stateLed, 96);
    lv_led_set_color(stateLed, lv_color_hex(0x888888));
    lv_led_off(stateLed);
}
