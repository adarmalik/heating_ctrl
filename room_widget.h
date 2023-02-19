#pragma once

#include <lvgl.h>
#include "Arduino.h"
#include "data_widgets.h"

class RoomWidget {
public:
  RoomWidget(lv_obj_t *parent);
  ~RoomWidget();

  void setRoomName(const char *name);
  const char* getRoomName();

  lv_obj_t *getBaseObject() { return baseObj; }

  void setWthId(const char *id){strncpy(wthId, id, sizeof(wthId));wthId[15]='\0';}
  const char* getWthId(){return wthId;}

  void setDrsId(const String id){drsId = id;}
  String getDrsId(){return drsId;}

  void refreshData();

  TempWidget *actTemp;
  ActiveProfileWidget *actProfile;
  TempWidget *spTemp;
  HumidityWidget *hum;
  ImageWidget *win;
  ImageWidget *heat;

private:
  lv_obj_t *baseObj;
  lv_obj_t *nameLabel;
  char wthId[16];
  String drsId;
};
