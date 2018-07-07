#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include "wifiConfig.c"

#define PIN_MOISTURE 0 // moisture sensor analog on PIN 0
#define PIN_DHT 4 // temp + humidity sensor
#define DHT_TYPE DHT22
#define INTERVAL_READ 2000
#define INTERVAL_CYCLE 100

DHT dht(PIN_DHT, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup");
  WiFi.begin(WIFI_SSID, WIFI_KEY);
    Serial.print("Waiting for WiFi connection..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(".");
  Serial.print("Successfully connected to ");
  Serial.println(WiFi.SSID());
  float moisture = getMoistureLevel(PIN_MOISTURE);
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature(false);
  float heatIndex = dht.computeHeatIndex(temp, humidity, false);
  while (isnan(humidity) || isnan(temp) || isnan(heatIndex)) {
    Serial.println("Error while reading sensor values, retrying");
    delay(2000);
    moisture = getMoistureLevel(PIN_MOISTURE);
    humidity = dht.readHumidity();
    temp = dht.readTemperature(false);
    heatIndex = dht.computeHeatIndex(temp, humidity, false);
  }
  logSensorStatus(moisture, temp, humidity, heatIndex);
  int httpCode = submitSensorStatus(POST_URL, moisture, temp, humidity, heatIndex);
  Serial.print("HTTP response: ");
  Serial.println(httpCode);
  ESP.deepSleep(300 * 1000000);
}

void loop() {
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

int submitSensorStatus(char*  url, float moisture, float temp, float humidity, float heatIndex) {
  HTTPClient http;
  String json = '{';
  json += '"moisture": ';
  json += moisture;
  json += ', "temp": ';
  json += temp;
  json += ', "humidity": ';
  json += humidity;
  json += '}';
  http.begin(url);
  int httpCode = http.POST(json);
  String payload = http.getString();
  if (httpCode < 0) {
    Serial.print("Error: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }
  http.end();
  return httpCode;
}
