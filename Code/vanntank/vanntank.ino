#include "UbidotsEsp32Mqtt.h"

const char *ubidots_token = "";
const char *wifi_ssid = "";
const char *wifi_pass = "";
const char *vannystem_esp32_node = "vannsystem";
const char *subscribe_soil1_variable = "soil1";
const char *subscribe_soil2_variable = "soil2";
const char *subscribe_soil3_variable = "soil3";

const int publish_frequency = 30*1000; // Update rate in seconds.
unsigned long timer = 0;
uint16_t subscribe_soil1;
uint16_t subscribe_soil2;
uint16_t subscribe_soil3;

Ubidots ubidots(ubidots_token);

void callback(char *topic, byte *payload, unsigned int length)
{
  //Store the payload
  payload[length] = '\0'; // Make payload a string by NULL terminating it.
  int Val = atoi((char *)payload);
  Serial.println(topic);
  //Check which topic payload has arrived from
  if (strstr(topic, "soil1"))
  {
    Serial.print("Soil sensor 1 har verdien: ");
    Serial.println(String(Val));
  }
  else if (strstr(topic, "soil2"))
  {
    Serial.print("Soil sensor 2 har verdien: ");
    Serial.println(String(Val));
  }
  else if(strstr(topic, "soil3"))
  {
    Serial.print("Soil sensor 3 har verdien: ");
    Serial.println(String(Val));
  }
  else
  {
    Serial.println("Confusing");
  }
}
void setup() 
{
  
  Serial.begin(115200);
  ubidots.connectToWifi(wifi_ssid, wifi_pass);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  
  // Variables to subscribe to
  subscribe_soil1 = ubidots.subscribeLastValue(vannystem_esp32_node, subscribe_soil1_variable);
  subscribe_soil2 = ubidots.subscribeLastValue(vannystem_esp32_node, subscribe_soil2_variable);
  subscribe_soil3 = ubidots.subscribeLastValue(vannystem_esp32_node, subscribe_soil3_variable);

  timer = millis();
}

void loop() 
{
  if(!ubidots.connected())
  {
    ubidots.reconnect();
    subscribe_soil1;
    subscribe_soil2;
    subscribe_soil3;
  }
  ubidots.loop();
}
