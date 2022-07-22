#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Blynk.h>
#define BLYNK_PRINT Serial

char auth[] = "YourAuthToken"; // Blynk Authentication Code
char ssid[] = "YourNetworkName"; // WiFi Name
char pass[] = "YourPassword"; // WiFi Password

#define DHTPIN D3          // Pin where the DHT11 is connected
DHT dht(DHTPIN, DHT11); // Object of DHT11
#define TRIGGER D1  // Trigger Pin
#define ECHO    D2  // Echo Pin

const int moisturePin = A0;             // Moisture Sensor Pin
const int motorPin = D0;    // Motor Pin
unsigned long interval = 10000;  // 10 Second Time Interval
unsigned long previous_time = 0;
unsigned long interval_1 = 1000; // 1 Second Time Interval
unsigned long previous_time1 = 0;
float moisturePercentage;              //Moisture Percenntage Reading
float h;                  // Humidity Reading
float t;                  //Temperature Reading

int height_tank = 105; // Height of the Tank in cm
long duration;  // Duration of Sound Waves
float v;                // Volume of the Tank in cubic cm
float distance;         // Height of the Water Level in cm
int r = 53.2;           // Radius of the Tank in cm
int cap;                // Capacity in liters
int actual_height;      // Actual Height in cm

void setup()
{
  Serial.begin(115200); // Baud Rate
  Blynk.begin(auth, ssid, pass); // For Blynk App
  delay(10);
  pinMode(motorPin, OUTPUT); // Motor Pin is Output Pin
  digitalWrite(motorPin, LOW); // Keep the Motor Off Initally
  dht.begin();
  pinMode(TRIGGER, OUTPUT); // Trigger Pin is Output Pin
  pinMode(ECHO, INPUT); // Echo Pin is Output Pin
}

void loop()
{
  unsigned long current_time = millis(); // Grab Current Time
  h = dht.readHumidity();     // Read humiduty
  t = dht.readTemperature();     // Read temperature

  if (isnan(h) || isnan(t)) // To check if DHT11 Sensor is Sending Values
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  moisturePercentage = ( 100.00 - ( (analogRead(moisturePin) / 1023.00) * 100.00 ) ); // Calculation of Mositure Percentage

  if ((unsigned long)(current_time - previous_time1) >= interval_1) // To check whether Time Duration is Greater than 1 Second
  {
    Serial.print("Soil Moisture is  = ");
    Serial.print(moisturePercentage);  // Print Moisture Percentage
    Serial.println("%");
    previous_time1 = millis();
  }

  // To Control the Motor Based on the Moisture Percentage
  
  if (moisturePercentage < 50)
  {
    digitalWrite(motorPin, HIGH);         // Turn on the Motor
  }
  if (moisturePercentage > 50 && moisturePercentage < 55)
  {
    digitalWrite(motorPin, HIGH);        // Turn on the Motor 
  }
  if (moisturePercentage > 56) 
  {
    digitalWrite(motorPin, LOW);          // Turn off the Motor
  }
  
  if ((unsigned long)(current_time - previous_time) >= interval) // To check whether Time Duration is Greater than 10 Seconds
  {
    sendBlynkData();           // Send the Data to Blynk App
    previous_time = millis();
  }

  // For Water Level Monitoring
  
  digitalWrite(TRIGGER, LOW);  // Keep Trigger Low
  delayMicroseconds(2); 
  
  digitalWrite(TRIGGER, HIGH); // Make Trigger High
  delayMicroseconds(10); 

  digitalWrite(TRIGGER, LOW); // Make Trigger Low
  duration = pulseIn(ECHO, HIGH); // Read the Time for Echo to Receive the Sound Waves
  distance= duration/58.2; // Calculate Distance by Using Speed of Sound (s = 340 m/s)
  delay (50);
  actual_height = height_tank - distance; // Calculate the Water Level
  v = ((3.14*(r*r))*(actual_height)); // Volume of Water
  cap = v/1000; // Capacity of the Tank in Litres
  Blynk.virtualWrite(V5, cap); // Send Data to Blynk App
  delay(200);
  Blynk.run();
}

void sendBlynkData()  // Send Data to Different Assigned Pins
{
  Blynk.virtualWrite(V0,moisturePercentage);
  Blynk.virtualWrite(V1,t);
  Blynk.virtualWrite(V2,h);
}
