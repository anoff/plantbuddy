#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include "wifiConfig.c"

#define PIN_MOISTURE 0 // moisture sensor analog on PIN 0
#define PIN_DHT 4 // temp + humidity sensor
#define DHT_TYPE DHT22
#define SLEEP_S 300
#define DEVICE_ID "plantbuddy1"

DHT dht(PIN_DHT, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup");
  wifiConnect(WIFI_SSID, WIFI_KEY);
  //ESP.deepSleep(SLEEP_S * 1000000);
}

void loop() {
  Serial.println("Sending heartbeat..");
  int httpBeat = submitHeartbeat(POST_URL, DEVICE_ID);
  Serial.print("Heartbeat HTTP Response: ");
  Serial.println(httpBeat);
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
  Serial.println("Sending sensor data..");
  int httpCode = submitSensorStatus(POST_URL, moisture, temp, humidity, heatIndex);
  Serial.print("Sensor data HTTP response: ");
  Serial.println(httpCode);
  delay(20000);
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

int submitSensorStatus(char* url, float moisture, float temp, float humidity, float heatIndex) {
  HTTPClient http;
  String json = "{";
  json += "\"moisture\": ";
  json += moisture;
  json += ", \"temp\": ";
  json += temp;
  json += ", \"humidity\": ";
  json += humidity;
  json += ", \"id\": \"";
  json += DEVICE_ID;
  json += "\"";
  json += ", \"type\": \"data\"";
  json += "}";
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

int submitHeartbeat(char*  url, char* id) {
  HTTPClient http;
  String json = "{\"msg\": \"Still alive \\o/\", \"id\": \"";
  json += id;
  json += "\"";
  json += ", \"type\": \"heartbeat\"";
  json += " }";
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

void wifiConnect(char* ssid, char* key) {
  WiFi.begin(ssid, key);
  Serial.print("Waiting for WiFi connection..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(".");
  Serial.print("Successfully connected to ");
  Serial.println(WiFi.SSID());
}
