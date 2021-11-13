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

int find_avg(int array[]){
  int sum  = 0;                          // Sets the sum as 0 
  for (int i = 0; i < num_readings; i++){sum += array[i];Serial.print(array[i]);Serial.println("  JEG KOMMER FRA FOR LOOP");}
  int avg = sum / num_readings;          // Finding average
  return avg;
}

void loop(){
  data[boot_counter] = analogRead(potPin);
  Serial.println(data[boot_counter]);
  Serial.println();
  if (boot_counter >= (num_readings - 1)){
    avg = find_avg(data);
    Serial.println();
    Serial.print("__________Average of the list of X values is   ");
    Serial.println(avg);

    boot_counter = 0;
  }
  boot_counter++;
  esp_deep_sleep_start();
}
