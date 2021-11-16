#include "UbidotsEsp32Mqtt.h"

const int led1 = 32;
const int led2 = 33;
const int led3 = 25;

const char *ubidots_token = "";
const char *wifi_ssid = "";
const char *wifi_pass = "";
const char *watersystem_esp32_node = "vannsystem";
const char *plant_node = "demo";
const char *subscribe_soil1_variable = "soil1";
const char *subscribe_soil2_variable = "soil2";
const char *subscribe_soil3_variable = "soil3";

const int publish_frequency = 10*1000; // Update rate in seconds.
unsigned long timer = 0;
uint16_t subscribe_soil1;
uint16_t subscribe_soil2;
uint16_t subscribe_soil3;

Ubidots ubidots(ubidots_token);
void watersystem(char *topic, byte *payload)
{
  int percentage = atoi((char *)payload);
  //This is a function used for finding where the payload comes from
  
  if(percentage < 70)
  {
    if(strstr(topic, "soil1"))
    {
      digitalWrite(led1, HIGH);
      Serial.println("Pump water to plant 1 for a few seconds");
      delay(2000);
      digitalWrite(led1, LOW);
    }
    else if(strstr(topic, "soil2"))
    {
      digitalWrite(led2, HIGH);
      Serial.println("Pump water to plant 2 for a few seconds");
      delay(2000);
      digitalWrite(led2, LOW);
    }
    else if(strstr(topic, "soil3"))
    {
      digitalWrite(led3, HIGH);
      Serial.println("Pump water to plant 3 for a few seconds");
      delay(2000);
      digitalWrite(led3, LOW);
    }
    else
    {
      Serial.println("Data not be used");
    }
  }
}
void callback(char *topic, byte *payload, unsigned int length)
{
  //Store the payload
  payload[length] = '\0'; // Make payload a string by NULL terminating it.
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
//  if(millis()-timer > publish_frequency)
//  {
    watersystem(topic, payload);
    timer = millis();
//  }
}
void setup() 
{
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  Serial.begin(115200);
  ubidots.connectToWifi(wifi_ssid, wifi_pass);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  
  // Variables to subscribe to
  subscribe_soil1 = ubidots.subscribeLastValue(plant_node, subscribe_soil1_variable);
  subscribe_soil2 = ubidots.subscribeLastValue(plant_node, subscribe_soil2_variable);
  subscribe_soil3 = ubidots.subscribeLastValue(plant_node, subscribe_soil3_variable);

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
