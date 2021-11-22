//Library
#include "UbidotsEsp32Mqtt.h"

//Defined temporary variables
#define sleep_time 5
#define seconds 1000000

//Components 
const int led1 = 32;
const int led2 = 33;
const int led3 = 25;

//Ubidots configs
const char *ubidots_token = "";
const char *wifi_ssid = "";
const char *wifi_pass = "";
const char *watersystem_esp32_node = "vannsystem";
const char *plant_node = "demo";
const char *subscribe_soil1_variable = "soil1";
const char *subscribe_soil2_variable = "soil2";
const char *subscribe_soil3_variable = "soil3";

uint16_t subscribe_soil1;
uint16_t subscribe_soil2;
uint16_t subscribe_soil3;

Ubidots ubidots(ubidots_token);

//Takes the data from payload and checks which sensor the payload came from. 
void watersystem(char *topic, byte *payload)
{
  int percentage = atoi((char *)payload);
  
  // The herbs needs a lot of water, so under 70% is a reliable checkmark.  
  if(percentage < 70)
  {
    // Using cstring to compare with the characters with following string.
    // If the characters involves "soil1", then we run functions for pump 1 that'll pump water for plant1. 
    if(strstr(topic, "soil1"))
    {
      // Here runs a simulation of a function that would normally water the patch of earth.
      // Led represents pump being active. 
      digitalWrite(led1, HIGH);
      Serial.println("Pump water to plant 1 for a few seconds");
      // Simulates an example where function takes longer than the timer for esp32 being awake
      for(int i = 0; i<50; i++)
      {
        Serial.println(i);
        delay(1000);
      }
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

  // Print out where the message came from and what the payload has stored. 
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // If the topic involves any word about soilsensor, then we send the data to compare the value and which sensor should get water if needed. 
  if(strstr(topic, "soil"))
  {
    watersystem(topic, payload);
  }
}

void setup() 
{
  Serial.begin(115200);

  //pinModes
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  //Put a timer on sleep mode.
  esp_sleep_enable_timer_wakeup(sleep_time*seconds);

  //Wifi connection, what function should run if message arrives from subscribed variables and etc. 
  delay(10);
  ubidots.connectToWifi(wifi_ssid, wifi_pass);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  
  // Variables for subscription. 
  subscribe_soil1 = ubidots.subscribeLastValue(plant_node, subscribe_soil1_variable);
  subscribe_soil2 = ubidots.subscribeLastValue(plant_node, subscribe_soil2_variable);
  subscribe_soil3 = ubidots.subscribeLastValue(plant_node, subscribe_soil3_variable);

}

unsigned long timer = millis();
long deep_sleep_frequency = 30000;

void loop() 
{
  if(!ubidots.connected())
  {
    ubidots.reconnect();
    subscribe_soil1;
    subscribe_soil2;
    subscribe_soil3;
  }
  if((millis()-timer) > deep_sleep_frequency)
  {
    Serial.println("Yeet");
    esp_deep_sleep_start();    
  }  
  ubidots.loop();
}
