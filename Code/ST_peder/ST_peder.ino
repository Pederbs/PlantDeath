//#include "QuickMedianLib.h"

#define num_readings 5
#define sleep_time 2
#define seconds 1000000         // Converts micro second to second

RTC_DATA_ATTR int boot_counter = 0;

int data[num_readings] = {0, 0, 0, 0, 0};
int dataI = 0;

int sortedData[num_readings] = {0, 0, 0, 0, 0};

int ledPin = 25;
int potPin = 32;

void setup()
{
	Serial.begin(115200);
  esp_sleep_enable_timer_wakeup(sleep_time * seconds);

  pinMode(ledPin, OUTPUT);
  pinMode(potPin, INPUT);

  digitalWrite(ledPin,HIGH);
}


void pushNewData(int d) {
    data[dataI] = d;

    dataI++;
    if(dataI > 4) {
        dataI = 0;
    }
}

int median() {
    for(int i = 0; i < 5; i++) {
        sortedData[i] = data[i];
    }

    // Use a sorting algorithm here to sort sortedData

    return sortedData[3];
}


void loop()
{
  boot_counter++;
  values[boot_counter] = analogRead(potPin);
  Serial.println(analogRead(potPin));
  if (boot_counter >= num_readings){
    Serial.print("Median of the list of 10 values is   ");
    long timeCount = micros();
    int med = QuickMedian<int>::GetMedian(values, valuesLength);
    timeCount = micros() - timeCount;
    Serial.print(med);
    Serial.println();
    Serial.print(timeCount);
    Serial.println("us");
    boot_counter = 0;
  }
  //esp_deep_sleep_start();
  delay(1000);
}
