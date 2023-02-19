#include "setpoint_dialog.h"
#include "ui.h"
#include "styles.h"
#include "constants.h"

SetPointMessage::SetPointMessage(const char* id, const float v): wthId(id), value(v){
    Serial.println(String("new message: ")+String(v));
}

String SetPointMessage::getMessage(){
    return String("{\"v\":") + String(value, 1) + String("}");
}

String SetPointMessage::getTopic(){
    return String(wthId) + String(SETPOINTTEMPERATURE PV);
}

ProgramMessage::ProgramMessage(const char* id, const uint8_t v): wthId(id), value(v){
    Serial.println(String("new message: ")+String(v));
}

String ProgramMessage::getMessage(){
    return String("{\"v\":") + String(value) + String("}");
}

String ProgramMessage::getTopic(){
    return String(wthId) + String(ACTIVEPROFILE PV);
}

static void set_point_button_cancel_handler(lv_event_t * e){
    if(!UI::getInstance().isBacklightOn())
        return;

    Serial.println("set_point_button_cancel_handler");
    UI::getInstance().showMainScreen();
}

static void set_point_button_event_handler(lv_event_t * e){
    if(!UI::getInstance().isBacklightOn())
        return;

    Serial.println("set_point_button_event_handler");
    auto dlg = UI::getInstance().getSetPointDialog();
    auto ui = &(UI::getInstance());
    ui->messages.push_back(std::shared_ptr<SetPointMessage>(new SetPointMessage(dlg->getCurrentWthId(), dlg->getCurrentSliderValue())));
    ui->messages.push_back(std::shared_ptr<ProgramMessage>(new ProgramMessage(dlg->getCurrentWthId(), dlg->getCurrentProgramValue())));
    ui->setMessageLed(true);
    ui->showMainScreen();
}

static void slider_event_cb(lv_event_t * e){
    if(!UI::getInstance().isBacklightOn())
        return;

    lv_obj_t * slider = lv_event_get_target(e);
    SetPointDialog* spDialog = (SetPointDialog*)e->user_data;

    Serial.print("Selected Option: ");
    Serial.println(lv_roller_get_selected(slider));
    float val = (lv_roller_get_selected(slider) / 2.0 ) + 10.0;
    Serial.println(val);
    // spDialog->setValue(val);
}

SetPointDialog::SetPointDialog(){
    currentRoom = nullptr;
    dialogScreen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(dialogScreen, lv_palette_darken(LV_PALETTE_GREY, 3), LV_PART_MAIN);
    lv_obj_set_style_text_color(dialogScreen, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_add_event_cb(dialogScreen, touch_event_cb, LV_EVENT_CLICKED, NULL);

    nameLabel = lv_label_create(dialogScreen);
    lv_obj_add_style(nameLabel, &(Styles::getInstance().RoomNameStyle), 0);
    lv_obj_align(nameLabel, LV_ALIGN_TOP_MID, 0, 30);

// Create a slider in the center of the display
    slider = lv_roller_create(dialogScreen);
    String options = "10";
    for(int i=21;i<61;i++){
        options.concat(" °C\n"+String(i/2.0,1));
    }
    options.concat(" °C");
    lv_roller_set_options(slider, options.c_str(), LV_ROLLER_MODE_NORMAL);
    lv_obj_align(slider, LV_ALIGN_CENTER, -100, -30);
    lv_obj_set_width(slider, 180);

    programRoller = lv_roller_create(dialogScreen);
    String programs = "1\n2\n3\n4\n5";
    lv_roller_set_options(programRoller, programs.c_str(), LV_ROLLER_MODE_NORMAL);
    lv_obj_align_to(programRoller, slider, LV_ALIGN_OUT_RIGHT_MID, 60, 0);
    lv_obj_set_width(programRoller, 60);
    lv_obj_add_style(programRoller, &(Styles::getInstance().DialogSliderStyle), LV_PART_MAIN);
    lv_obj_add_style(programRoller, &(Styles::getInstance().DialogSliderStyle), LV_PART_INDICATOR);
    lv_obj_add_style(programRoller, &(Styles::getInstance().DialogSliderStyle), LV_PART_KNOB);
    lv_obj_add_style(programRoller, &(Styles::getInstance().DialogSliderActiveStyle), LV_PART_SELECTED);

    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, (void*)this); // partially contructed?
    lv_obj_add_flag(slider, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_style(slider, &(Styles::getInstance().DialogSliderStyle), LV_PART_MAIN);
    lv_obj_add_style(slider, &(Styles::getInstance().DialogSliderStyle), LV_PART_INDICATOR);
    lv_obj_add_style(slider, &(Styles::getInstance().DialogSliderStyle), LV_PART_KNOB);

    lv_obj_add_style(slider, &(Styles::getInstance().DialogSliderActiveStyle), LV_PART_SELECTED);
    lv_roller_set_visible_row_count(slider, 4);

    lv_obj_t * btn_okay = lv_btn_create(dialogScreen);
    lv_obj_set_size(btn_okay, 200, 80);
    lv_obj_add_event_cb(btn_okay, set_point_button_event_handler, LV_EVENT_CLICKED, (void*)this);
    lv_obj_add_flag(btn_okay, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_align_to(btn_okay, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);
    lv_obj_add_style(btn_okay, &(Styles::getInstance().DialogButtonStyle), 0);

    lv_obj_t * label = lv_label_create(btn_okay);
    lv_label_set_text(label, "Set");
    lv_obj_add_style(label, &(Styles::getInstance().RoomWidgetNameStyle), 0);
    lv_obj_center(label);

    lv_obj_t * btn_cancel = lv_btn_create(dialogScreen);
    lv_obj_set_size(btn_cancel, 200, 80);
    lv_obj_add_event_cb(btn_cancel, set_point_button_cancel_handler, LV_EVENT_CLICKED, (void*)this);
    lv_obj_add_flag(btn_cancel, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_align_to(btn_cancel, btn_okay, LV_ALIGN_OUT_RIGHT_MID, 40, 0);
    lv_obj_add_style(btn_cancel, &(Styles::getInstance().DialogButtonStyle), 0);

    lv_obj_t * label2 = lv_label_create(btn_cancel);
    lv_label_set_text(label2, "Cancel");
    lv_obj_add_style(label2, &(Styles::getInstance().RoomWidgetNameStyle), 0);
    lv_obj_center(label2);
}

const char* SetPointDialog::getCurrentWthId(){
    if(currentRoom){
        return currentRoom->getWthId();
    } else {
        return nullptr;
    }
}


void SetPointDialog::setup(const char* name, const float value){
    lv_label_set_text(nameLabel, name);
    setValue(value);
}

void SetPointDialog::setup(const char* name, const float temperatureValue, const uint8_t programValue){
    lv_label_set_text(nameLabel, name);
    setValue(temperatureValue);
    setProgramValue(programValue);
}

void SetPointDialog::setValue(const float value){
    int tmp = round((value - 10)*2);
    Serial.print("roller set index");
    Serial.println(tmp);
    Serial.println(value);
    lv_roller_set_selected(slider, tmp, LV_ANIM_OFF);
}

float SetPointDialog::getCurrentSliderValue(){
    float v = (lv_roller_get_selected(slider) / 2.0 ) + 10.0;
    Serial.println("getCurrentSliderValue: " + String(v));
    return v;
}

void SetPointDialog::setProgramValue(const uint8_t value){
    lv_roller_set_selected(programRoller, value-1, LV_ANIM_OFF);
}

const uint8_t SetPointDialog::getCurrentProgramValue(){
    return (lv_roller_get_selected(programRoller) + 1);
}

void SetPointDialog::show(RoomWidget *widget){
    currentRoom = widget;
    setup(widget->getRoomName(), widget->spTemp->getData());
    show();
}

void SetPointDialog::show(){
    lv_scr_load_anim(dialogScreen, LV_SCR_LOAD_ANIM_FADE_IN, 0, 0, false);
}
