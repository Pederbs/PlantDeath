
// Include Libraries
#include "UbidotsEsp32Mqtt.h"
#include <Adafruit_AHTX0.h>
#include <WiFi.h>

// Define Constants
const char *UBIDOTS_TOKEN = "BBFF-zAB17mfcz5sGxEz17GPb5cSsyHPkRH";            // Put here your Ubidots TOKEN
const char *WIFI_SSID = "NSB_INTERAKTIV";                // Put here your Wi-Fi SSID
const char *WIFI_PASS = "987654321pink";              // Put here your Wi-Fi password
const char *PUBLISH_DEVICE_LABEL = "demo";       // Put here your Device label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL1 = "soil1";   // Put here your Variable label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL2 = "soil2";   // Put here your Variable label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL3 = "soil3";   // Put here your Variable label to which data  will be published
const char *PUBLISH_VARIABLE_LABEL_TEMP = "temperature";
const char *PUBLISH_VARIABLE_LABEL_HUMID = "humidity";

#define num_readings 10
#define sleep_time 60
#define seconds 1000000

Adafruit_AHTX0 aht;                          // Defines the function used to retrive temp and humi

// VALUES
int led1 = 0;
int led2 = 0;
int led3 = 0;
int soil_avg1 = 0;
int soil_avg2 = 0;
int soil_avg3 = 0;
float temp_avg = 0;
float humid_avg = 0;
int not_connected_counter = 0;

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
RTC_DATA_ATTR float soil1_array[num_readings];
RTC_DATA_ATTR float soil2_array[num_readings];
RTC_DATA_ATTR float soil3_array[num_readings];
RTC_DATA_ATTR float humidity_array[num_readings];
RTC_DATA_ATTR float temperature_array[num_readings];
// Variables that store last value published
RTC_DATA_ATTR int last_soil_avg1;
RTC_DATA_ATTR int last_soil_avg2;
RTC_DATA_ATTR int last_soil_avg3;
RTC_DATA_ATTR float last_humid_avg;
RTC_DATA_ATTR float last_temp_avg;


Ubidots ubidots(UBIDOTS_TOKEN);


float find_avg(float array[]){
  /* 
   * A function that finds the average of the sensor array given as argument 
   */
  float sum  = 0;                          // Sets the sum as 0 
  for (int i = 0; i < num_readings; i++){  // Uses a for loop to iterate througt all values in the array
    sum += array[i];                       // Adds value to the sum 
    // Serial.print("   ");Serial.println(array[i]);
  }
  float avg = sum / num_readings;          // Finding average
  return avg;
}

void WiFi_status_control(){
  /* 
   * A function that checks WiFi connection and resets the device if
   * it doesn't connect within 30 seconds.  
   */
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi connecting...");
    delay(500);
    not_connected_counter += 1;
    if(not_connected_counter > 60) { // Reset board if not connected after 30s
      Serial.println("Resetting due to Wifi not connecting...");
      ESP.restart();
    }
  }
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

  //Setting up ext lib's
  aht.begin();                // Starts the function to retreve temp and humid

  // Boot counter still in progress. Gather soil data and store it.
  if(boot_counter != (num_readings)){
    sensors_event_t humidity,temp;
    aht.getEvent(&humidity, &temp);
    
    // Populates the value arrays with fresh mesurements
    soil1_array[boot_counter] = analogRead(soil_pin1);
    soil2_array[boot_counter] = analogRead(soil_pin2);
    soil3_array[boot_counter] = analogRead(soil_pin3);
    humidity_array[boot_counter] = humidity.relative_humidity;
    temperature_array[boot_counter] = temp.temperature;

    // Makes the ESP go back to sleep
    boot_counter++;
    esp_deep_sleep_start();
  }

  // Check if the desired amounts of boots has been reached, 
  // then it will find the average value of each sensor array and publish to UBIDOTS.
  else{
    boot_counter = 0;                     // Resets the boot counter for new data collection

    Serial.begin(115200);
    delay(100);                           // Wait for serial to initialize

    // Set up WiFi connection and Ubidots
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    WiFi_status_control();
    Serial.println("Wifi connected");
    ubidots.setup();
    ubidots.reconnect();
  }
  // Find average values of the stored array values
  temp_avg  = find_avg(temperature_array);
  humid_avg = find_avg(humidity_array);
  // maps the 12-bit average into percentage moisture, where 0% is completly dry and 100% is soaked
  soil_avg1 = map(find_avg(soil1_array),0,4095,0,100); 
  soil_avg2 = map(find_avg(soil2_array),0,4095,0,100);
  soil_avg3 = map(find_avg(soil3_array),0,4095,0,100);
  
  // Pair the values with the publish variables respectivly if the value is different from last value
  if (soil_avg1 != last_soil_avg1) {
    ubidots.add(PUBLISH_VARIABLE_LABEL1, soil_avg1);
    last_soil_avg1 = soil_avg1;
  }
  if (soil_avg2 != last_soil_avg2) {
    ubidots.add(PUBLISH_VARIABLE_LABEL2, soil_avg2);
    last_soil_avg2 = soil_avg2;
  }
  if (soil_avg3 != last_soil_avg3) {
    ubidots.add(PUBLISH_VARIABLE_LABEL3, soil_avg3);
    last_soil_avg3 = soil_avg3;
  }
  if (temp_avg != last_temp_avg) {
    ubidots.add(PUBLISH_VARIABLE_LABEL_TEMP, temp_avg);
    last_temp_avg = temp_avg;
  }
  if (humid_avg != last_humid_avg) {
    ubidots.add(PUBLISH_VARIABLE_LABEL_HUMID, humid_avg);
    last_humid_avg = humid_avg;
  }

  ubidots.publish(PUBLISH_DEVICE_LABEL);

  Serial.print("Right soil-sens:    ");Serial.println(soil_avg1);
  Serial.print("Middle soil-sens:    ");Serial.println(soil_avg2);
  Serial.print("Left soil-sens:    ");Serial.println(soil_avg3);
  Serial.print("Temperature:    "); Serial.print(temp_avg); Serial.println(" degrees C"); 
  Serial.print("Humidity:    "); Serial.print(humid_avg); Serial.println("% rH"); 
  
  esp_deep_sleep_start();
}

void loop(){            
  Serial.println("PASS PÅ NAA ER JEG I VOID LOOP");
}