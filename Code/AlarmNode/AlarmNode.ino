//Library
#include "UbidotsEsp32Mqtt.h"

//Defined temporary variables
#define sleep_time 120
#define seconds 1000000

//Components 
const int buzzerLo = 32;
const int buzzerHi = 33;
int critical_temp = 10;
bool alarm_status = 0;

// setting PWM properties
const int freqLo = 800;
const int freqHi = 1000;
const int ledChannelLo = 0;
const int ledChannelHi = 2;
const int resolution = 1;


unsigned long timer;
long wait_timer = 2000;
long alarm_duration = 300000; // 5 minutes


//Ubidots configs
const char *ubidots_token = "BBFF-zAB17mfcz5sGxEz17GPb5cSsyHPkRH";
const char *wifi_ssid = "kameraBad2";
const char *wifi_pass = "9D2Remember";
const char *alarm_esp32_node = "alarm";
const char *plant_node = "demo";
const char *subscribe_temp_variable = "temperature";

Ubidots ubidots(ubidots_token);

//Takes the data from payload and checks which sensor the payload came from. 
void alarm(char *topic, byte *payload){
  int temp = atoi((char *)payload);
  alarm_status = 0;
  
  // Sets of the alarm it the temperature is under 10 degrees 
  if(temp < critical_temp){
    alarm_status = 1;
    wait_timer = alarm_duration;

    ledcSetup(ledChannelLo, freqLo, resolution);
    ledcSetup(ledChannelHi, freqHi, resolution);
    ledcAttachPin(buzzerLo, ledChannelLo);
    ledcAttachPin(buzzerHi, ledChannelHi);
  }
  //return alarm_status;
}

void callback(char *topic, byte *payload, unsigned int length){
  //Store the payload
  payload[length] = '\0'; // Make payload a string by NULL terminating it.

  /*
  // Print out where the message came from and what the payload has stored. 
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  //*/

  // If the topic involves any word about soilsensor, then we send the data to compare the value and which sensor should get water if needed. 
  if(strstr(topic, "temp"))
  {
    alarm(topic, payload);
  }
}

void setup() {
  //Serial.begin(115200);
  //Put a timer on sleep mode.
  esp_sleep_enable_timer_wakeup(sleep_time*seconds);

  //Wifi connection, what function should run if message arrives from subscribed variables and etc. 
  delay(10);
  ubidots.connectToWifi(wifi_ssid, wifi_pass);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  
  // Variables for subscription. 
  ubidots.subscribeLastValue(plant_node, subscribe_temp_variable);
  timer = millis();
}


void loop(){
  if(!ubidots.connected()){
    ubidots.reconnect();
    ubidots.subscribeLastValue(plant_node, subscribe_temp_variable);
  }
  if((millis()-timer) > wait_timer){
    //Serial.println("    Going to sleep");
    esp_deep_sleep_start(); 
  }
  if(alarm_status == 1){
    ledcWrite(ledChannelHi, 0);
    ledcWrite(ledChannelLo, 1);
    delay(250);
  
    ledcWrite(ledChannelLo, 0);
    ledcWrite(ledChannelHi, 1);
    delay(250);
  } 
  ubidots.loop();
}