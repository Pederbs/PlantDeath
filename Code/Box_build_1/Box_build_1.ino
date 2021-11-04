/******************************************
 *
 * This example works for both Industrial and STEM users.
 *
 * Developed by Jose Garcia, https://github.com/jotathebest/
 *
 * ****************************************/

/****************************************
 * Include Libraries
 ****************************************/
#include "UbidotsEsp32Mqtt.h"

/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBFF-zAB17mfcz5sGxEz17GPb5cSsyHPkRH";            // Put here your Ubidots TOKEN
const char *WIFI_SSID = "iProbe";                // Put here your Wi-Fi SSID
const char *WIFI_PASS = "Torpedor";                // Put here your Wi-Fi password
const char *PUBLISH_DEVICE_LABEL = "demo";     // Put here your Device label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL1 = "soil1";   // Put here your Variable label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL2 = "soil2";   // Put here your Variable label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL3 = "soil3";   // Put here your Variable label to which data  will be published
//const char *SUBSCRIBE_DEVICE_LABEL = "";   // Replace with the device label to subscribe to
//const char *SUBSCRIBE_VARIABLE_LABEL = ""; // Replace with the variable label to subscribe to

const int PUBLISH_FREQUENCY = 5000; // Update rate in millisecondsx

float value1 = 0;
float value2 = 0;
float value3 = 0;

unsigned long timer = 1000;
int analogPin1 = 34; // Pin used to read data from GPIO34 ADC_CH6. HØYERE
int analogPin2 = 35; // Pin used to read data from GPIO35 ADC_CH?. MIDT
int analogPin3 = 32; // Pin used to read data from GPIO32 ADC_C?. VENSTRE

Ubidots ubidots(UBIDOTS_TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  //ubidots.subscribeLastValue(SUBSCRIBE_DEVICE_LABEL, SUBSCRIBE_VARIABLE_LABEL); // Insert the device and variable's Labels, respectively

  timer = millis();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!ubidots.connected())
  {
    ubidots.reconnect();
    //ubidots.subscribeLastValue(SUBSCRIBE_DEVICE_LABEL, SUBSCRIBE_VARIABLE_LABEL); // Insert the device and variable's Labels, respectively
  }
  if ((millis() - timer) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    value1 = analogRead(analogPin1);
    value2 = analogRead(analogPin2);
    value3 = analogRead(analogPin3);
    ubidots.add(PUBLISH_VARIABLE_LABEL1, value1); // Insert your variable Labels and the value to be sent
    ubidots.add(PUBLISH_VARIABLE_LABEL2, value2); // Insert your variable Labels and the value to be sent
    ubidots.add(PUBLISH_VARIABLE_LABEL3, value3); // Insert your variable Labels and the value to be sent
    ubidots.publish(PUBLISH_DEVICE_LABEL);
    Serial.print("Right soil-sens:    ");Serial.println(value1);
    Serial.print("Right soil-sens:    ");Serial.println(value2);
    Serial.print("Right soil-sens:    ");Serial.println(value3);
    timer = millis();
  }

  ubidots.loop();
}
