#define num_readings 5
#define sleep_time 2
#define seconds 1000000         // Converts micro second to second

RTC_DATA_ATTR int boot_counter = 0;

RTC_DATA_ATTR int data[num_readings];

int avg = 0;

int ledPin = 25;
int potPin = 32;

void setup(){
	Serial.begin(115200);
  esp_sleep_enable_timer_wakeup(sleep_time * seconds);

  pinMode(ledPin, OUTPUT);
  pinMode(potPin, INPUT);

  //digitalWrite(ledPin,HIGH);
}

void loop(){
  data[boot_counter] = analogRead(potPin);
  Serial.println(analogRead(potPin));
  if (boot_counter >= num_readings){
    long timeCount = micros();
    int sum = 0;
    for (int i = 0; i <= boot_counter; i++){
      sum = sum + data[i];
      Serial.print("Summen er: ");Serial.println(sum);
      Serial.println();
      Serial.print("Data er: ");Serial.println(data[i]);
    }
    avg = sum / num_readings;
    
    timeCount = micros() - timeCount;
    Serial.print("Average of the list of X values is   ");
    Serial.print(avg);
    Serial.println();
    Serial.print(timeCount);
    Serial.println("us");
    boot_counter = 0;
  }
  boot_counter++;
  esp_deep_sleep_start();
  //delay(1000);
}
