#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

#define PIN_MOISTURE 0 // moisture sensor analog on PIN 0
#define PIN_DHT 4 // temp + humidity sensor
#define DHT_TYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup");
  WiFi.begin("gastnetz", "LOLwhat");
    Serial.print("Waiting for connection..");
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
  if (msSinceLastRead >= 2000) {
    float moisture = getMoistureLevel(PIN_MOISTURE);
    float humidity = dht.readHumidity();
    float temp = dht.readTemperature(false);
    float heatIndex = dht.computeHeatIndex(temp, humidity, false);
    Serial.print("Moisture Level: ");
    Serial.print(moisture);
    Serial.print("\t");
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print("\t");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("\t");
    Serial.print("Head Index: ");
    Serial.print(heatIndex);
    Serial.println("");
    msSinceLastRead = 0;
  }
 } else {
  Serial.println("WiFi connection broke");   
 }
  delay(100);
  msSinceLastRead += 100;
}

float getMoistureLevel(int PIN) {
  float moisture;
  moisture = analogRead(PIN);
  moisture = moisture/10;
  return moisture;
}