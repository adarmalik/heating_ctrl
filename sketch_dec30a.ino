#include "secrets.h"
#include "screen_setup.h"
#include "ui.h"
#include "constants.h"

#include <map>
#include <WiFi.h>
#include <PubSubClient.h>

std::map<String, Subscriber*> subscriptions;
WiFiClient client;
PubSubClient mqttClient(client);

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Connected to AP successfully!");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  UI::getInstance().connected(true);
  connectToMqtt();
  UI::getInstance().refreshTimeLabel();
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.println("Trying to Reconnect");
  if (info.wifi_sta_disconnected.reason == WIFI_REASON_NO_AP_FOUND) {
    Serial.print("Scanning networks ... ");
    int n = WiFi.scanNetworks();
    Serial.print("found ");
    Serial.println(n);
    for(int i=0; i<n; i++){
      Serial.println(WiFi.SSID(i));
    }
  }
  UI::getInstance().connected(false);
  UI::getInstance().mqttConnected(false);
  connectToWifi();
}

void onMqttConnect(bool sessionPresent){
  Serial.println("Connected to MQTT broker");
  UI::getInstance().mqttConnected(true);
  for (auto it=subscriptions.begin(); it!=subscriptions.end(); ++it){
    mqttClient.subscribe(it->first.c_str(), 1);
  }
  UI::getInstance().connected(true);
}

void onMqttMessage(char* topic, byte* payload, unsigned int len){
   Serial.println(topic);
   if(subscriptions.count(topic) == 0){
     Serial.println("no subscription found");
   }else{
     auto dp = subscriptions[topic];
     dp->parseData((char*)payload);
   }
 }

void connectToMqtt(){
  Serial.println("Connecting to MQTT...");
  if(mqttClient.connect("esp")){
    onMqttConnect(true);
  } else {
    delay(250);
  }
}

void connectToWifi(){
  WiFi.mode(WIFI_MODE_STA);
  char* ssid = WIFINAME;
  char *password = PASSWORD;
  WiFi.begin(ssid, password);
}

void create_subscription(RoomWidget *w, String wth_id, String drs_id){
  String start = "device/status/";
  if(!wth_id.isEmpty()){
    w->setWthId(wth_id.c_str());
    subscriptions[start + wth_id + ACTUALTEMPERATURE] = w->actTemp;    
    subscriptions[start + wth_id + SETPOINTTEMPERATURE] = w->spTemp;
    subscriptions[start + wth_id + HUMIDITY] = w->hum;
    subscriptions[start + wth_id + ACTIVEPROFILE] = w->actProfile;    
    subscriptions[start + wth_id + WINDOWSTATE] = w->win;
  }

  if(!drs_id.isEmpty()){
    subscriptions[start + drs_id + DRSSTATE] = w->heat;
    w->setDrsId(drs_id);
  }
}

void setup(){
  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  Serial.begin(115200);
  while (!Serial);
   
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(onMqttMessage);
  
  if(!init_screen()){
    Serial.println("Panic");
    delay(30000);
    ESP.restart();
  }

  RoomWidget **rw = UI::getInstance().getRoomWidgets();
  rw[0]->setRoomName(NAME_0);
  rw[1]->setRoomName(NAME_1);
  rw[2]->setRoomName(NAME_2);
  rw[3]->setRoomName(NAME_3);
  rw[4]->setRoomName(NAME_4);
  rw[5]->setRoomName(NAME_5);
  rw[6]->setRoomName(NAME_6);
  rw[7]->setRoomName(NAME_7);
  rw[8]->setRoomName(NAME_8);
  rw[9]->setRoomName(NAME_9);

  create_subscription(rw[0], WTH_0, DRS_0);
  create_subscription(rw[1], WTH_1, DRS_1);
  create_subscription(rw[2], WTH_2, DRS_2);
  create_subscription(rw[3], WTH_3, DRS_3);
  create_subscription(rw[4], WTH_4, DRS_4);
  create_subscription(rw[5], WTH_5, DRS_5);
  create_subscription(rw[6], WTH_6, DRS_6);
  create_subscription(rw[7], WTH_7, DRS_7);
  create_subscription(rw[8], WTH_8, DRS_8);
  create_subscription(rw[9], WTH_9, DRS_9);

  connectToWifi();
}

void loop(){
  static uint32_t counter = 0;
  lv_timer_handler(); /* let the GUI do its work */
  
  delay(UI::getInstance().getRefreshTime());
  
  if(mqttClient.connected()){
    mqttClient.loop();
    counter = 0;
  } else {
    UI::getInstance().mqttConnected(false);
    counter += UI::getInstance().getRefreshTime();
    if(counter > 5000){
      connectToMqtt();
    }
  }
}
