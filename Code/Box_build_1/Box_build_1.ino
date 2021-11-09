
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

int value1 = 0;
int value2 = 0;
int value3 = 0;
float value_temp = 0;
float value_humi = 0;

unsigned long timer = 1000;
int analogPin1 = 34; // Pin used to read data from GPIO34 ADC_CH6. HØYERE
int analogPin2 = 35; // Pin used to read data from GPIO35 ADC_CH?. MIDT
int analogPin3 = 32; // Pin used to read data from GPIO32 ADC_C?. VENSTRE

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
  aht.begin();                    // Starts the function to retreve temp and humid
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
    value1 = analogRead(analogPin1);
    value2 = analogRead(analogPin2);
    value3 = analogRead(analogPin3);
    
    ubidots.add(PUBLISH_VARIABLE_LABEL1, value1); // Insert your variable Labels and the value to be sent
    ubidots.add(PUBLISH_VARIABLE_LABEL2, value2); // Insert your variable Labels and the value to be sent
    ubidots.add(PUBLISH_VARIABLE_LABEL3, value3); // Insert your variable Labels and the value to be sent
    
    ubidots.add(PUBLISH_VARIABLE_LABEL_TEMP, value_temp); // Insert your variable Labels and the value to be sent
    ubidots.add(PUBLISH_VARIABLE_LABEL_HUMI, value_humi); // Insert your variable Labels and the value to be sent

    ubidots.publish(PUBLISH_DEVICE_LABEL);
    Serial.print("Right soil-sens:    ");Serial.println(value1);
    Serial.print("Middle soil-sens:    ");Serial.println(value2);
    Serial.print("Left soil-sens:    ");Serial.println(value3);
    Serial.print("Temperature:    "); Serial.print(temp.temperature); Serial.println(" degrees C");  //for feilsøking
    Serial.print("Humidity:    "); Serial.print(humidity.relative_humidity); Serial.println("% rH");  //for feilsøking
    timer = millis();
  }

  ubidots.loop();
}
