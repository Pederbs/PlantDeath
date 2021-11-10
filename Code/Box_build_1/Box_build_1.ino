
// Include Libraries
#include "UbidotsEsp32Mqtt.h"
#include <Adafruit_AHTX0.h>

// Define Constants
const char *UBIDOTS_TOKEN = "BBFF-zAB17mfcz5sGxEz17GPb5cSsyHPkRH";            // Put here your Ubidots TOKEN
const char *WIFI_SSID = "G-boii Ziggzagg";                // Put here your Wi-Fi SSID
const char *WIFI_PASS = "987654321pink";              // Put here your Wi-Fi password
const char *PUBLISH_DEVICE_LABEL = "demo";       // Put here your Device label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL1 = "soil1";   // Put here your Variable label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL2 = "soil2";   // Put here your Variable label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL3 = "soil3";   // Put here your Variable label to which data  will be published

const char *PUBLISH_VARIABLE_LABEL_TEMP = "temperature";
const char *PUBLISH_VARIABLE_LABEL_HUMI = "humidity";
//const char *SUBSCRIBE_DEVICE_LABEL = "";   // Replace with the device label to subscribe to
//const char *SUBSCRIBE_VARIABLE_LABEL = ""; // Replace with the variable label to subscribe to

Adafruit_AHTX0 aht;                          // Defines the function used to retrive temp and humi

const int PUBLISH_FREQUENCY = 120000; // Update every 2 minute


// VALUES
int led1 = 0;
int led2 = 0;
int led3 = 0;
int soil_value1 = 0;
int soil_value2 = 0;
int soil_value3 = 0;
float value_temp = 0;
float value_humi = 0;
unsigned long timer = 1000;

// INPUT PINS
int soil_pin1 = 34; // Pin used to read data from GPIO34 ADC_CH6. HØYERE
int soil_pin2 = 35; // Pin used to read data from GPIO35 ADC_CH7. MIDT
int soil_pin3 = 32; // Pin used to read data from GPIO32 ADC_CH4. VENSTRE

// OUTPUT PINS
int led_pin1 = 13; // Pin used to light led1
int led_pin2 = 12; // Pin used to light led2
int led_pin3 = 14; // Pin used to light led3


Ubidots ubidots(UBIDOTS_TOKEN);

 // Auxiliar Functions
void callback(char *topic, byte *payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Main Functions

void setup(){
  pinMode(soil_pin1, INPUT);
  pinMode(soil_pin2, INPUT);
  pinMode(soil_pin3, INPUT);

  pinMode(led_pin1, OUTPUT);
  pinMode(led_pin2, OUTPUT);
  pinMode(led_pin3, OUTPUT);

  aht.begin();                // Starts the function to retreve temp and humid
  Serial.begin(115200);
  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  //ubidots.subscribeLastValue(SUBSCRIBE_DEVICE_LABEL, SUBSCRIBE_VARIABLE_LABEL); // Insert the device and variable's Labels, respectively

  timer = millis();
}

void loop(){
  // put your main code here, to run repeatedly:
  if (!ubidots.connected()){
    ubidots.reconnect();
    //ubidots.subscribeLastValue(SUBSCRIBE_DEVICE_LABEL, SUBSCRIBE_VARIABLE_LABEL); // Insert the device and variable's Labels, respectively
  }
  if ((millis() - timer) > PUBLISH_FREQUENCY){ // triggers the routine every 5 seconds
    sensors_event_t humidity,temp;
    aht.getEvent(&humidity, &temp);            // populate temp objects with fresh data

    value_temp = temp.temperature;
    value_humi = humidity.relative_humidity;
    soil_value1 = analogRead(soil_pin1);
    soil_value2 = analogRead(soil_pin2);
    soil_value3 = analogRead(soil_pin3);
    
    ubidots.add(PUBLISH_VARIABLE_LABEL1, soil_value1); // Insert your variable Labels and the value to be sent
    ubidots.add(PUBLISH_VARIABLE_LABEL2, soil_value2); // Insert your variable Labels and the value to be sent
    ubidots.add(PUBLISH_VARIABLE_LABEL3, soil_value3); // Insert your variable Labels and the value to be sent
    
    ubidots.add(PUBLISH_VARIABLE_LABEL_TEMP, value_temp); // Insert your variable Labels and the value to be sent
    ubidots.add(PUBLISH_VARIABLE_LABEL_HUMI, value_humi); // Insert your variable Labels and the value to be sent

    ubidots.publish(PUBLISH_DEVICE_LABEL);
    Serial.print("Right soil-sens:    ");Serial.println(soil_value1);
    Serial.print("Middle soil-sens:    ");Serial.println(soil_value2);
    Serial.print("Left soil-sens:    ");Serial.println(soil_value3);
    Serial.print("Temperature:    "); Serial.print(temp.temperature); Serial.println(" degrees C");  //for feilsøking
    Serial.print("Humidity:    "); Serial.print(humidity.relative_humidity); Serial.println("% rH");  //for feilsøking
    timer = millis();
  }

  ubidots.loop();
}
