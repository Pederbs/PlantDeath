//Library
#include "UbidotsEsp32Mqtt.h"

//Components 
const int buzzerLo = 32;
const int buzzerHi = 33;
const int button_pin = 35;
int critical_temp = 10; // Critical temperature value
bool alarm_status = 0;
bool button_status = 0;

//Delays
unsigned long nominal_alarm_time = 300000; // 300 000 sec = 5 min
unsigned long alarm_timer;

//Setting PWM properties
const int freqLo = 800;
const int freqHi = 1000;
const int ledChannelLo = 0;
const int ledChannelHi = 2;
const int resolution = 1; // Amplitude og PWM is set to resolution of 1 bit 

//Ubidots configs
const char *ubidots_token = "BBFF-zAB17mfcz5sGxEz17GPb5cSsyHPkRH";
const char *wifi_ssid = "iProbe"; // Insert WiFi name here
const char *wifi_pass = "Torpedor"; // Insert WiFi password here
const char *alarm_esp32_node = "alarm";
const char *plant_node = "demo";
const char *subscribe_temp_variable = "temperature";

Ubidots ubidots(ubidots_token);

//Takes the data from payload and checks which sensor the payload came from. 
void alarm(char *topic, byte *payload){
  int temp = atoi((char *)payload);
  // Activates the alarm it the temperature is less that the critical temperature 
  if(temp < critical_temp){alarm_status = 1;}
  else{alarm_status = 0;} // Deactivates the alerm if the previous if statment is not true
}

void callback(char *topic, byte *payload, unsigned int length){
  //Store the payload
  payload[length] = '\0'; // Make payload a string by NULL terminating it.
  // If the topic involves any word about temp, the data is sent to compare with critical_temp
  if(strstr(topic, "temp")){alarm(topic, payload);}
}

void setup() {
  Serial.begin(115200);
  pinMode(button_pin, INPUT);
  ledcSetup(ledChannelLo, freqLo, resolution);
  ledcSetup(ledChannelHi, freqHi, resolution);
  ledcAttachPin(buzzerLo, ledChannelLo);
  ledcAttachPin(buzzerHi, ledChannelHi);

  //Wifi connection, what function should run if message arrives from subscribed variables and etc. 
  delay(10);
  ubidots.connectToWifi(wifi_ssid, wifi_pass);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  
  // Subscribing to the temp variable from Ubidots
  ubidots.subscribeLastValue(plant_node, subscribe_temp_variable);
}


void loop(){
  if(!ubidots.connected()){ // Reconnect to Ubidots if it is not already connected
    ubidots.reconnect();
    ubidots.subscribeLastValue(plant_node, subscribe_temp_variable);
  }
  if (alarm_status == 1){// Checks if the button is activated only if the alarm is active
    button_status = digitalRead(button_pin); // Idle state 0
    Serial.println(button_status);
  }
  if (button_status == 1 && (millis() - alarm_timer) > nominal_alarm_time){// Button or timer deactivated the alarm
    Serial.println("ALARM Skrus AV");
    alarm_timer = millis();
    ledcWrite(ledChannelHi, 0);
    ledcWrite(ledChannelLo, 0);
    alarm_status = 0;
  }
  if(alarm_status == 1){// Running one cycle of the alarm
    ledcWrite(ledChannelHi, 0);
    ledcWrite(ledChannelLo, 1);
    delay(250);
  
    ledcWrite(ledChannelLo, 0);
    ledcWrite(ledChannelHi, 1);
    delay(250);
  }
  else if(alarm_status == 0){// Ensures that the alarm is indeed off
    ledcWrite(ledChannelHi, 0);
    ledcWrite(ledChannelLo, 0);
  }
  ubidots.loop();
}