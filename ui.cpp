#include "ui.h"
#include "styles.h"
#include "screen_setup.h"
#include <HTTPClient.h>
#include <math.h>
#include "constants.h"
#include "secrets.h"

extern void touch_event_cb(lv_event_t * e){
    Serial.println("touch event cb");
    UI::getInstance().setBacklightOn(true);
    UI::getInstance().resumeBacklightTimer();
    UI::getInstance().resetBacklightTimer();
}

void publish_messages(lv_timer_t *t){
    static HTTPClient http;
    static uint8_t errorCounter = 0;
    auto ui = &(UI::getInstance());
    if(!ui->messages.empty()){
        auto msg = ui->messages.back();
        String ep("http://");
        ep.concat(MQTT_HOST);
        ep.concat(URL);
        ep.concat(msg->getTopic());
        Serial.println(msg->getMessage());
        Serial.println(ep);
        http.begin(ep);
        http.addHeader("Content-Type", "application/json");
        int rc = http.PUT(msg->getMessage());
        if(rc == 200){
            ui->messages.pop_back();
            errorCounter = 0;
        } else {
            Serial.print("HTTP result: ");
            Serial.println(rc);
            errorCounter++;
            if(errorCounter>10){
                ESP.restart();
            }
        }
        http.end();
    }
    if(ui->messages.empty()){
        ui->setMessageLed(false);
    }
}

static void refresh_cb(lv_event_t * e){
  RoomWidget **rw = UI::getInstance().getRoomWidgets();
  String url;
  HTTPClient http;
  for(int i=0;i<8;i++){
      auto w = rw[i];
      w->actTemp->init(&http, String(String(MQTT_HOST) + String(URL) + w->getWthId() + ACTUALTEMPERATURE PV).c_str());
      w->spTemp->init(&http, String(String(MQTT_HOST) + String(URL) + w->getWthId() + SETPOINTTEMPERATURE PV).c_str());
      w->hum->init(&http, String(String(MQTT_HOST) + String(URL) + w->getWthId() + HUMIDITY PV).c_str());
      w->actProfile->init(&http, String(String(MQTT_HOST) + String(URL) + w->getWthId() + ACTIVEPROFILE PV).c_str());
      w->win->init(&http, String(String(MQTT_HOST) + String(URL) + w->getWthId() + WINDOWSTATE PV).c_str());
      w->heat->init(&http, String(String(MQTT_HOST) + String(URL) + w->getDrsId() + DRSSTATE PV).c_str());
  }
}

static void reconnection_cb(lv_event_t * e){
    WiFi.disconnect(false, true);
}

void backlight_timer_cb(lv_timer_t * timer){
    Serial.println("backlight timer fired");
    UI::getInstance().setBacklightOn(false);
    UI::getInstance().pauseBacklightTimer();
    UI::getInstance().closeConfigDialog();
}

void time_set_cb(lv_timer_t *t){
    UI::getInstance().refreshTimeLabel();
}

static void time_btn_clicked_cb(lv_event_t *e){
    UI::getInstance().refreshTimeLabel();
}

static void mbox_switchoff_cb(lv_event_t * e){
    lv_obj_t * obj = lv_event_get_current_target(e);
    String btn = lv_msgbox_get_active_btn_text(obj);
    UI::getInstance().closeConfigDialog();
    if(btn == "Display"){
        backlight_timer_cb(NULL);
    } else if(btn == "All"){
        esp_sleep_enable_timer_wakeup(30*1000000);
        delay(50);
        esp_deep_sleep_start();
    }
}

static void time_btn_lpress_cb(lv_event_t *e){
    UI::getInstance().showConfigDialog();
}

UI::UI(){
    configTzTime(TZ_INFO, NTP_SERVER);
    configDialog = nullptr;

    backlightTimer = lv_timer_create(backlight_timer_cb, BACKLIGHT_SWITCHOFF_TIME_SEC, NULL);
    messageTimer = lv_timer_create(publish_messages, 500, NULL);
    ntpTimer = lv_timer_create(time_set_cb, 10000, NULL);

    mainScreen = lv_scr_act();
    lv_obj_set_style_bg_color(mainScreen, lv_color_hex(0x202020), LV_PART_MAIN);
    //lv_obj_set_style_bg_color(mainScreen, lv_color_black(), LV_PART_MAIN);
    lv_obj_add_event_cb(mainScreen, touch_event_cb, LV_EVENT_CLICKED, NULL);

    spDialog = new SetPointDialog();

    for(uint32_t i = 0; i < 10; i++) {
        roomWidgets[i]= new RoomWidget(mainScreen);
        if(i==0){
            lv_obj_align(roomWidgets[i]->getBaseObject(), LV_ALIGN_TOP_LEFT, 5, 5);
        } else if(i==5){
            lv_obj_align_to(roomWidgets[i]->getBaseObject(), roomWidgets[0]->getBaseObject(), LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
        } else {
            lv_obj_align_to(roomWidgets[i]->getBaseObject(), roomWidgets[i-1]->getBaseObject(), LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        }
    }
    reconnectButton = lv_btn_create(mainScreen);
    lv_obj_set_size(reconnectButton, 155, 60);
    lv_obj_add_flag(reconnectButton, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_align_to(reconnectButton, roomWidgets[5]->getBaseObject(), LV_ALIGN_OUT_BOTTOM_MID, 0, 7);
    lv_obj_add_style(reconnectButton, &(Styles::getInstance().MainButtonStyle), 0);
    lv_obj_add_event_cb(reconnectButton, reconnection_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * reconnectLabel = lv_label_create(reconnectButton);
    lv_label_set_text(reconnectLabel, LV_SYMBOL_WIFI);
    lv_obj_add_style(reconnectLabel, &(Styles::getInstance().RoomWidgetNameStyle), 0);
    lv_obj_align(reconnectLabel, LV_ALIGN_CENTER, -20, 0);

    connectionLed = lv_led_create(reconnectButton);
    lv_obj_align_to(connectionLed, reconnectButton, LV_ALIGN_OUT_RIGHT_MID, -60, 5);
    lv_led_set_brightness(connectionLed, 128);
    lv_obj_set_size(connectionLed, 12, 12);
    lv_led_set_color(connectionLed, lv_color_hex(0xAAFF40));
    lv_led_off(connectionLed);

    mqttLed = lv_led_create(reconnectButton);
    lv_obj_align_to(mqttLed, reconnectButton, LV_ALIGN_OUT_RIGHT_MID, -30, 5);
    lv_led_set_brightness(mqttLed, 128);
    lv_obj_set_size(mqttLed, 12, 12);
    lv_led_set_color(mqttLed, lv_color_hex(0x40BB40));
    lv_led_off(mqttLed);

    lv_obj_t *timeButton = lv_btn_create(mainScreen);
    lv_obj_add_flag(timeButton , LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_size(timeButton, 470, 60);
    lv_obj_add_style(timeButton, &(Styles::getInstance().MainButtonStyle), 0);
    lv_obj_align_to(timeButton, reconnectButton, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_add_event_cb(timeButton, time_btn_clicked_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(timeButton, time_btn_lpress_cb, LV_EVENT_LONG_PRESSED, NULL);

    timeLabel = lv_label_create(timeButton);
    lv_label_set_text(timeLabel, "- - - - -");
    lv_obj_add_style(timeLabel, &(Styles::getInstance().RoomWidgetNameStyle), 0);
    lv_obj_set_style_text_align(timeLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(timeLabel, LV_ALIGN_CENTER, -20, 0);

    refreshButton = lv_btn_create(mainScreen);
    lv_obj_add_flag(refreshButton, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_size(refreshButton, 155, 60);
    lv_obj_align_to(refreshButton, roomWidgets[9]->getBaseObject(), LV_ALIGN_OUT_BOTTOM_MID, 0, 7);
    lv_obj_add_style(refreshButton, &(Styles::getInstance().MainButtonStyle), 0);
    lv_obj_add_event_cb(refreshButton, refresh_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * refreshLabel = lv_label_create(refreshButton);
    lv_label_set_text(refreshLabel, LV_SYMBOL_REFRESH);
    lv_obj_add_style(refreshLabel, &(Styles::getInstance().RoomWidgetNameStyle), 0);
    lv_obj_align(refreshLabel, LV_ALIGN_CENTER, -20, 0);

    messagesLed = lv_led_create(refreshButton);
    lv_obj_align_to(messagesLed, connectionLed, LV_ALIGN_OUT_RIGHT_MID, 560, 0);
    lv_led_set_color(messagesLed, lv_color_hex(0xAAAA00));
    lv_led_set_brightness(messagesLed, 128);
    lv_obj_align_to(messagesLed, refreshButton, LV_ALIGN_OUT_RIGHT_MID, -40, 0);
    lv_led_off(messagesLed);
}

void UI::showConfigDialog(){
    static const char * btns[] ={"Display", "All", ""};
    configDialog = lv_msgbox_create(NULL, "Switch Off", "", btns, true);
    lv_obj_set_size(configDialog, 450, 300);
    lv_obj_set_size(lv_msgbox_get_btns(configDialog), 400, 200);
    lv_obj_add_style(configDialog, &(Styles::getInstance().RoomWidgetBaseStyle), 0);
    lv_obj_add_style(configDialog, &(Styles::getInstance().RoomWidgetBaseStyle), 0);
    lv_obj_add_style(configDialog, &(Styles::getInstance().RoomWidgetBaseStyle), 0);
    lv_obj_add_style(lv_msgbox_get_btns(configDialog), &(Styles::getInstance().MainButtonStyle), LV_PART_ITEMS);
    lv_obj_add_style(lv_msgbox_get_close_btn(configDialog), &(Styles::getInstance().MainButtonStyle), 0);
    lv_obj_center(lv_msgbox_get_btns(configDialog));

    lv_obj_add_event_cb(configDialog, mbox_switchoff_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(configDialog);
    lv_obj_add_flag(configDialog , LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(lv_msgbox_get_btns(configDialog), LV_OBJ_FLAG_EVENT_BUBBLE);
}

void UI::closeConfigDialog(){
    if(configDialog){
        lv_msgbox_close(configDialog);
        configDialog = nullptr;
    }
}

void UI::setBacklightOn(bool on){
    pinMode(TFT_BL, OUTPUT);
    if(on){
        digitalWrite(TFT_BL, HIGH);
        resetBacklightTimer();
        setRefreshTime(5);
    } else {
        digitalWrite(TFT_BL, LOW);
        setRefreshTime(100);
    }
    blOn = on;
}

void trySleep(struct tm timeinfo){
    if(timeinfo.tm_year > 100){
        if(timeinfo.tm_hour == 1){
            if(timeinfo.tm_min < 5){
                  esp_sleep_enable_timer_wakeup(5*60*60*1000000);
                  delay(50);
                  esp_deep_sleep_start();
            }
        }
    }
}

void UI::refreshTimeLabel(){
    time_t now;
    struct tm timeinfo;
    time(&now);

    localtime_r(&now, &timeinfo);
    char strftime_buf[64];

    strftime(strftime_buf, sizeof(strftime_buf), "%a, %d.%m.%y %H:%M", &timeinfo);
    setTime(String(strftime_buf));
    trySleep(timeinfo);
}

void UI::setMessageLed(bool on){
    if(on){
        lv_led_on(messagesLed);
    }else{
        lv_led_off(messagesLed);
    }
}

void UI::connected(bool on){
    if(on){
        lv_led_on(connectionLed);
    }else{
        lv_led_off(connectionLed);
    }
}

void UI::mqttConnected(bool on){
    if(on){
        lv_led_on(mqttLed);
    }else{
        lv_led_off(mqttLed);
    }
}

void UI::showMainScreen(){
    lv_scr_load(mainScreen);
}

void UI::showSetPointDialog(RoomWidget *widget){
    spDialog->setup(widget->getRoomName(), widget->spTemp->getData(), widget->actProfile->getData());
    spDialog->show(widget);
}
