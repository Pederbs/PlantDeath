//Library
#include "UbidotsEsp32Mqtt.h"

//Components 
const int buzzerLo = 32;
const int buzzerHi = 33;
const int button_pin = 35;
int critical_temp = 10;
bool alarm_status = 0;
bool button_status = 0;

//Delays
unsigned long alarm_delay = 300000; // alarm er av i 5 min
unsigned long alarm_timer;

//Setting PWM properties
const int freqLo = 800;
const int freqHi = 1000;
const int ledChannelLo = 0;
const int ledChannelHi = 2;
const int resolution = 1;

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
  // Sets of the alarm it the temperature is under 10 degrees 
  if(temp < critical_temp){alarm_status = 1;}
  else{alarm_status = 0;}
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

  // If the topic involves any word about temperature, then we send the data to compare the value amd evaluate if the alarm should be on
  if(strstr(topic, "temp")){alarm(topic, payload);}
}

void setup() {
  pinMode(button_pin, INPUT);
  //Serial.begin(115200);
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
  
  // Variables for subscription. 
  ubidots.subscribeLastValue(plant_node, subscribe_temp_variable);
}


void loop(){
  if(!ubidots.connected()){
    ubidots.reconnect();
    ubidots.subscribeLastValue(plant_node, subscribe_temp_variable);
  }
  if (alarm_status == 1){//skjekker om knapp er påtrykket bare om alarm er aktiv
    button_status = digitalRead(button_pin); // idle state 0
  }
  if (button_status == 1 && (millis() - alarm_timer) > alarm_delay){//knapp eller timer skrur av alarmen
    //Serial.println("  Knapp paatrykket // alarm slaaes av");
    alarm_timer = millis();
    ledcWrite(ledChannelHi, 0);
    ledcWrite(ledChannelLo, 0);
    alarm_status = 0;
  }
  if(alarm_status == 1){// alarm på
    ledcWrite(ledChannelHi, 0);
    ledcWrite(ledChannelLo, 1);
    delay(250);
  
    ledcWrite(ledChannelLo, 0);
    ledcWrite(ledChannelHi, 1);
    delay(250);
  }
  else if(alarm_status == 0){//forsikrer om at alarm er av
    ledcWrite(ledChannelHi, 0);
    ledcWrite(ledChannelLo, 0);
  }
  ubidots.loop();
}