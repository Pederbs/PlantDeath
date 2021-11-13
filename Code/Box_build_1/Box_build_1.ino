
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

#define num_readings 5
#define sleep_time 5
#define seconds 1000000
//const char *SUBSCRIBE_DEVICE_LABEL = "";   // Replace with the device label to subscribe to
//const char *SUBSCRIBE_VARIABLE_LABEL = ""; // Replace with the variable label to subscribe to

Adafruit_AHTX0 aht;                          // Defines the function used to retrive temp and humi

const int PUBLISH_FREQUENCY = 10000; // Update every 10 sec




// VALUES
int led1 = 0;
int led2 = 0;
int led3 = 0;
int soil_avg1 = 0;
int soil_avg2 = 0;
int soil_avg3 = 0;
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




// RTC_DATA_ATTR used to store data in RTC memory. 
// A counter that tracks how many times ESP32 has been woken up from deep sleep
RTC_DATA_ATTR int boot_counter = 0;

// Array Variables that store sensor measurements
RTC_DATA_ATTR int soil1_array[num_readings];
RTC_DATA_ATTR int soil2_array[num_readings];
RTC_DATA_ATTR int soil3_array[num_readings];
RTC_DATA_ATTR float humidity_array[num_readings];
RTC_DATA_ATTR float temperature_array[num_readings];



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

int find_avg(int array[]){
  /* 
   * A function that finds the average of the sensor array given as argument 
   */
  int sum  = 0;                          // Sets the sum as 0 
  for (int i = 0; i < num_readings; i++){  // Uses a for loop to iterate througt all values in the array
    sum += array[i];                       // Adds value to the sum 
  }
  int avg = sum / num_readings;          // Finding average
  return avg;
}

int get_soil(int pin){
  /*
   * A function that finds the percent of soilmoisture in the soil
   */
  int value = analogRead(pin);              // Reads the pin value
  int percent = map(value,0,4095,0,100);  // Finding the value in percent
  return percent;
}

// Main Functions

void setup(){
  pinMode(soil_pin1, INPUT);
  pinMode(soil_pin2, INPUT);
  pinMode(soil_pin3, INPUT);

  pinMode(led_pin1, OUTPUT);
  pinMode(led_pin2, OUTPUT);
  pinMode(led_pin3, OUTPUT);

  //Defining wakeup reasons
  esp_sleep_enable_timer_wakeup(sleep_time*seconds);
  boot_counter++;

  // Boot counter still in progress. Gather soil data and store it.
  if(boot_counter != num_readings){
    // Populates the value arrays with fresh mesurements
    soil1_array[boot_counter] = get_soil(soil_pin1);
    soil2_array[boot_counter] = get_soil(soil_pin2);
    soil3_array[boot_counter] = get_soil(soil_pin3);
    // Makes the ESP go back to sleep
    esp_deep_sleep_start();
  }

  // Check if the desired amounts of boots has been reached, 
  // then it will find the average value of each sensor array and publish to UBIDOTS.
  if (boot_counter == num_readings){
    boot_counter = 0;                     // Resets the boot counter for new data collection

    Serial.begin(115200);
    // ubidots.setDebug(true);  // uncomment this to make debug messages available
    ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
    ubidots.setCallback(callback);
    ubidots.setup();
    ubidots.reconnect();
    //ubidots.subscribeLastValue(SUBSCRIBE_DEVICE_LABEL, SUBSCRIBE_VARIABLE_LABEL); // Insert the device and variable's Labels, respectively
  }
  aht.begin();                // Starts the function to retreve temp and humid
  
  

  timer = millis();

  
}

void loop(){
  // put your main code here, to run repeatedly:
  if (!ubidots.connected()){
    ubidots.reconnect();
    //ubidots.subscribeLastValue(SUBSCRIBE_DEVICE_LABEL, SUBSCRIBE_VARIABLE_LABEL); // Insert the device and variable's Labels, respectively
  }
  
    sensors_event_t humidity,temp;
    aht.getEvent(&humidity, &temp);            // populate temp objects with fresh data

    value_temp = temp.temperature;
    value_humi = humidity.relative_humidity;
    soil_avg1 = find_avg(soil1_array);
    soil_avg2 = find_avg(soil2_array);
    soil_avg3 = find_avg(soil3_array);
    
    ubidots.add(PUBLISH_VARIABLE_LABEL1, soil_avg1); // Insert your variable Labels and the value to be sent
    ubidots.add(PUBLISH_VARIABLE_LABEL2, soil_avg2); // Insert your variable Labels and the value to be sent
    ubidots.add(PUBLISH_VARIABLE_LABEL3, soil_avg3); // Insert your variable Labels and the value to be sent
    
    ubidots.add(PUBLISH_VARIABLE_LABEL_TEMP, value_temp); // Insert your variable Labels and the value to be sent
    ubidots.add(PUBLISH_VARIABLE_LABEL_HUMI, value_humi); // Insert your variable Labels and the value to be sent

    ubidots.publish(PUBLISH_DEVICE_LABEL);
    Serial.print("Right soil-sens:    ");Serial.println(soil_avg1);
    Serial.print("Middle soil-sens:    ");Serial.println(soil_avg2);
    Serial.print("Left soil-sens:    ");Serial.println(soil_avg3);
    Serial.print("Temperature:    "); Serial.print(temp.temperature); Serial.println(" degrees C");  //for feilsøking
    Serial.print("Humidity:    "); Serial.print(humidity.relative_humidity); Serial.println("% rH");  //for feilsøking
    timer = millis();
  
  esp_deep_sleep_start();
  //ubidots.loop();
}
