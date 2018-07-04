#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

#define PIN_MOISTURE 0 // moisture sensor analog on PIN 0
#define PIN_DHT 4 // temp + humidity sensor
#define DHT_TYPE DHT22
#define INTERVAL_READ 2000
#define INTERVAL_CYCLE 100

DHT dht(PIN_DHT, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup");
  WiFi.begin("gastnetz", "lol");
    Serial.print("Waiting for WiFi connection..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(".");
  Serial.print("Successfully connected to ");
  Serial.println(WiFi.SSID());
}

int msSinceLastRead = 0;
void loop() {
 if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
  if (msSinceLastRead >= INTERVAL_READ) {
    float moisture = getMoistureLevel(PIN_MOISTURE);
    float humidity = dht.readHumidity();
    float temp = dht.readTemperature(false);
    float heatIndex = dht.computeHeatIndex(temp, humidity, false);
    if (isnan(humidity) || isnan(temp) || isnan(heatIndex)) {
      Serial.println("Error while reading sensor values, retrying");
      msSinceLastRead = max(INTERVAL_READ - 1000, 0);
      return;
    }
    logSensorStatus(moisture, temp, humidity, heatIndex);
    msSinceLastRead = 0;
  }
 } else {
  Serial.println("WiFi connection broke");   
 }
  delay(INTERVAL_CYCLE);
  msSinceLastRead += INTERVAL_CYCLE;
}

float getMoistureLevel(int PIN) {
  float moisture;
  moisture = analogRead(PIN);
  moisture = moisture/10;
  return moisture;
}

void logSensorStatus(float moisture, float temp, float humidity, float heatIndex) {
  Serial.print("Moisture Level: ");
  Serial.print(moisture);
  Serial.print("\t");
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("\t");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("\t");
  Serial.print("Heat Index: ");
  Serial.print(heatIndex);
  Serial.println("");
}