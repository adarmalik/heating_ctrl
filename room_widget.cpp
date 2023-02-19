#include "room_widget.h"
#include "styles.h"
#include "Arduino.h"
#include "ui.h"


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_WINDOW_CLOSED_30PX
#define LV_ATTRIBUTE_IMG_WINDOW_CLOSED_30PX
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_WINDOW_CLOSED_30PX uint8_t window_closed_30px_map[] = {
  0xff, 0xff, 0xff, 0xfc,
  0x80, 0x00, 0x00, 0x04,
  0x80, 0x00, 0x00, 0x04,
  0x80, 0x00, 0x00, 0x04,
  0x8f, 0xff, 0xff, 0xc4,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x04, 0x44,
  0x88, 0x00, 0x02, 0x44,
  0x88, 0x00, 0x09, 0x44,
  0x88, 0x00, 0x04, 0xc4,
  0x88, 0x00, 0x02, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x88, 0x00, 0x00, 0x44,
  0x8f, 0xff, 0xff, 0xc4,
  0x80, 0x00, 0x00, 0x04,
  0x80, 0x00, 0x00, 0x04,
  0x80, 0x00, 0x00, 0x04,
  0xff, 0xff, 0xff, 0xfc,
};

const lv_img_dsc_t window_closed_30px = {
  LV_IMG_CF_ALPHA_1BIT,
  0,
  0,
  30,
  30,
  120,
  window_closed_30px_map
};

#ifndef LV_ATTRIBUTE_IMG_HOT_30PX
#define LV_ATTRIBUTE_IMG_HOT_30PX
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_HOT_30PX uint8_t hot_30px_map[] = {
  0x00, 0x0f, 0xc0, 0x00,
  0x00, 0x7f, 0xf8, 0x00,
  0x01, 0xff, 0xfe, 0x00,
  0x03, 0xff, 0xff, 0x00,
  0x0f, 0xff, 0xff, 0xc0,
  0x0f, 0xff, 0xff, 0xc0,
  0x1f, 0xfc, 0xff, 0xe0,
  0x3f, 0xf8, 0x7f, 0xf0,
  0x3f, 0xf8, 0x7f, 0xf0,
  0x7f, 0xf8, 0x3f, 0xf8,
  0x7f, 0xf0, 0x27, 0xf8,
  0x7f, 0xf0, 0x07, 0xf8,
  0xff, 0xa0, 0x03, 0xfc,
  0xff, 0x80, 0x03, 0xfc,
  0xff, 0x01, 0x01, 0xfc,
  0xff, 0x01, 0x81, 0xfc,
  0xfe, 0x01, 0xc1, 0xfc,
  0xfe, 0x07, 0xc1, 0xfc,
  0x7e, 0x0f, 0xc1, 0xf8,
  0x7e, 0x0f, 0xe1, 0xf8,
  0x7e, 0x1f, 0xe3, 0xf8,
  0x3f, 0x1f, 0xe3, 0xf0,
  0x3f, 0x1f, 0xe7, 0xf0,
  0x1f, 0x9f, 0xcf, 0xe0,
  0x0f, 0xff, 0xff, 0xc0,
  0x07, 0xff, 0xff, 0x80,
  0x03, 0xff, 0xff, 0x00,
  0x01, 0xff, 0xfe, 0x00,
  0x00, 0x7f, 0xf8, 0x00,
  0x00, 0x0f, 0xc0, 0x00,
};

const lv_img_dsc_t hot_30px = {
  LV_IMG_CF_ALPHA_1BIT,
  0,
  0,
  30,
  30,
  120,
  hot_30px_map,
};

LV_IMG_DECLARE(window_closed_30px);
LV_IMG_DECLARE(hot_30px);


void room_widget_clicked(lv_event_t * e){
    if(!UI::getInstance().isBacklightOn())
        return;

    RoomWidget* widget = (RoomWidget*)e->user_data;
    UI::getInstance().showSetPointDialog(widget);
}

RoomWidget::RoomWidget(lv_obj_t *parent){
    baseObj = lv_obj_create(parent);
    lv_obj_set_size(baseObj, 154, 200);
    lv_obj_add_flag(baseObj, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_set_scrollbar_mode(baseObj, LV_SCROLLBAR_MODE_OFF);

    static lv_coord_t column_dsc[] = {70, 70, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {30, 30, 30, 30, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_align(baseObj, LV_GRID_ALIGN_SPACE_AROUND, LV_GRID_ALIGN_END);

    lv_obj_set_style_grid_column_dsc_array(baseObj, column_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(baseObj, row_dsc, 0);

    lv_obj_set_layout(baseObj, LV_LAYOUT_GRID);

    lv_obj_add_style(baseObj, &(Styles::getInstance().RoomWidgetBaseStyle), 0);

    nameLabel = lv_label_create(baseObj);
    lv_obj_add_style(nameLabel, &(Styles::getInstance().RoomWidgetNameStyle), 0);
    lv_obj_add_event_cb(baseObj, room_widget_clicked, LV_EVENT_CLICKED, (void*)this);
    lv_obj_add_flag(baseObj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(nameLabel, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_width(nameLabel, 135);
    lv_obj_set_style_text_align(nameLabel, LV_TEXT_ALIGN_CENTER, 0);

    actTemp = new TempWidget(baseObj);
    hum = new HumidityWidget(baseObj);
    spTemp = new TempWidget(baseObj);
    actProfile = new ActiveProfileWidget(baseObj);
    heat = new ImageWidget(baseObj, &hot_30px, 0xFFAAAA);
    win = new ImageWidget(baseObj, &window_closed_30px, 0xFFFFFF);
    // heat = new LedWidget(baseObj, "H", 0xFFFFFF);

    //lv_obj_set_grid_cell(child, column_align, column_pos, column_span, row_align, row_pos, row_span)
    lv_obj_set_grid_cell(nameLabel,LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(actTemp->getLvObj(),LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(hum->getLvObj(),LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(spTemp->getLvObj(),LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(actProfile->getLvObj(),LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(win->getLvObj(),LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(heat->getLvObj(),LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_clear_flag(baseObj, LV_OBJ_FLAG_SCROLLABLE);
}

void RoomWidget::setRoomName(const char* name){
    lv_label_set_text_fmt(nameLabel, "%s", name);
}

const char* RoomWidget::getRoomName(){
    return lv_label_get_text(nameLabel);
}

