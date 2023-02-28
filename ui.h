#pragma once

#include <cstdlib>
#include <ArduinoJson.h>
#include <lvgl.h>
#include <sys/types.h>
#include "vector"
#include <HTTPClient.h>
#include "data_widgets.h"
#include "room_widget.h"
#include "setpoint_dialog.h"

extern void touch_event_cb(lv_event_t *);

class UI {
public:
  static UI &getInstance() {
    static UI instance;
    return instance;
  };
  UI(UI const &) = delete;
  void operator=(UI const &) = delete;

  u_int8_t getRefreshTime() const {return refreshTime;}
  void setRefreshTime(u_int8_t t){refreshTime=t;}

  RoomWidget **getRoomWidgets() { return roomWidgets; }
  SetPointDialog *getSetPointDialog() { return spDialog; }

  void resetBacklightTimer() {lv_timer_reset(backlightTimer);}
  void pauseBacklightTimer() {lv_timer_pause(backlightTimer);}
  void resumeBacklightTimer() {lv_timer_resume(backlightTimer);}

  void setBacklightOn(bool on);
  bool isBacklightOn() {return blOn;}

  void showMainScreen();
  void showSetPointDialog(RoomWidget*);

  void connected(bool);
  void mqttConnected(bool);

  void setMessageLed(bool);
  void refreshTimeLabel();
  void setTime(String time){lv_label_set_text(timeLabel, time.c_str());}

  void closeConfigDialog();
  void showConfigDialog();

  std::vector<std::shared_ptr<Message> > messages;

private:
  UI();

  lv_obj_t *mainScreen;
  RoomWidget *roomWidgets[10];
  SetPointDialog *spDialog;
  lv_obj_t *configDialog;

  lv_timer_t *backlightTimer;
  lv_timer_t *messageTimer;
  lv_timer_t *ntpTimer;

  lv_obj_t *reconnectButton;
  lv_obj_t *refreshButton;
  lv_obj_t *connectionLed;
  lv_obj_t *mqttLed;
  lv_obj_t *messagesLed;
  lv_obj_t *timeLabel;

  u_int8_t refreshTime = 5;
  bool blOn = true;
};
