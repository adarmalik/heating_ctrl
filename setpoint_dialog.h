#pragma once

#include <lvgl.h>
#include "room_widget.h"

class Message{
public:
    virtual String getMessage() = 0;
    virtual String getTopic() = 0;
};

class ProgramMessage : public Message {
public:
    ProgramMessage(const char* id, const uint8_t v);
    virtual ~ProgramMessage(){};
    virtual String getMessage();
    virtual String getTopic();

private:
    String wthId;
    uint8_t value;
};

class SetPointMessage : public Message {
public:
    SetPointMessage(const char* id, const float v);
    virtual ~SetPointMessage(){};
    virtual String getMessage();
    virtual String getTopic();

private:
    String wthId;
    float value;
};

class SetPointDialog {
public:
  SetPointDialog();
  void setup(const char *name, const float value);
  void setup(const char *name, const float temperatureValue, const uint8_t programValue);
  void show();
  void show(RoomWidget*);
  void setValue(const float value);
  void setProgramValue(const uint8_t value);
  const uint8_t getCurrentProgramValue();
  float getCurrentSliderValue();
  bool programValueHasChanged();
  bool sliderValueHasChanged();
  const char* getCurrentWthId();

private:
  lv_obj_t *dialogScreen;
  lv_obj_t *slider;
  lv_obj_t *programRoller;
  lv_obj_t *nameLabel;
  lv_obj_t *sliderLabel;
  RoomWidget *currentRoom;
  float initialTemp;
  uint8_t initiatlProgram;
};
