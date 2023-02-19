#pragma once

#include <ArduinoJson.h>
#include <lvgl.h>
#include <HTTPClient.h>

class Subscriber {
public:
  virtual void parseData(const char *payload) = 0;
};

template <typename T> class Widget : public Subscriber {
public:
  virtual void parseData(const char *payload) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
      Serial.println("parsing json failed");
    } else {
      data = doc["v"].as<T>();
      redraw();
    }
  }
  virtual void init(HTTPClient *client, const char* url){
    client->begin(url);
    int rc = client->GET();
    Serial.print(url);
    if(rc == 200){
      parseData(client->getString().c_str());
      Serial.println(" ok");
    } else {
        Serial.println(rc);
    }
    client->end();
  }
  virtual void redraw() = 0;
  virtual void setData(const T d) {
    data = d;
    redraw();
  }
  virtual T getData(){ return data; }
  virtual lv_obj_t *getLvObj() { return obj; }

protected:
  T data;
  lv_obj_t *obj;
};

//ACTIVE_PROFILE
class ActiveProfileWidget : public Widget<int> {
public:
  ActiveProfileWidget(lv_obj_t *parent);
  void redraw() { lv_label_set_text_fmt(obj, "%i", data); };
};

class TempWidget : public Widget<float> {
public:
  TempWidget(lv_obj_t *parent);
  void redraw() { lv_label_set_text_fmt(obj, "%.1f °C", data); };
};

class LedWidget : public Widget<bool> {
public:
  LedWidget(lv_obj_t *, String, uint32_t);
  void redraw() {
    if(data){
      lv_led_set_color(stateLed, lv_color_hex(activeColor));
      lv_led_on(stateLed);
    } else {
      lv_led_set_color(stateLed, lv_color_hex(0x888888));
      lv_led_off(stateLed);
    }
  };

  virtual lv_obj_t *getLedObj() { return stateLed; }
private:
  lv_obj_t *stateLed;
  uint32_t activeColor;
};

class ImageWidget : public Widget<bool> {
public:
  ImageWidget(lv_obj_t *, const lv_img_dsc_t*, uint32_t);
  void redraw() {
    if(data){
      lv_obj_set_style_img_recolor(obj, lv_color_hex(activeColor), 0);
      // lv_led_set_color(stateLed, lv_color_hex(activeColor));
      // lv_led_on(stateLed);
    } else {
      lv_obj_set_style_img_recolor(obj, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    }
  };

  virtual lv_obj_t *getLedObj() { return stateLed; }
private:
  lv_obj_t *stateLed;
  uint32_t activeColor;
};

class HumidityWidget : public Widget<float> {
public:
  HumidityWidget(lv_obj_t *);
  void redraw() { lv_label_set_text_fmt(obj, "%.0f %%", data); };
};
