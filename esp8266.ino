#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include "esp8266.secrets.c"

#define PIN_MOISTURE_READ 0 // moisture sensor analog on PIN 0
#define PIN_MOISTURE_POWER 12 // D6
#define PIN_DHT_READ 4 // D2, temp + humidity sensor
#define PIN_DHT_POWER 13 // D7
#define DHT_TYPE DHT22
#define SLEEP_S 900 // how many seconds to sleep between readings
#define DEVICE_ID "plantbuddy1"
#define MAX_READING_RETRIES 10 // maximum number of retries when getting NaN values from sensor

DHT dht(PIN_DHT_READ, DHT_TYPE);

void setup() {
  pinMode(PIN_DHT_POWER, OUTPUT);
  pinMode(PIN_MOISTURE_POWER, OUTPUT);
  Serial.begin(115200);
  Serial.println("Starting setup");
  delay(100);
  digitalWrite(PIN_DHT_POWER, HIGH);
  digitalWrite(PIN_MOISTURE_POWER, HIGH);
  wifiConnect(WIFI_SSID, WIFI_KEY);
  // submit heartbeat
  Serial.println("Sending heartbeat..");
  int httpBeat = submitHeartbeat(POST_URL, DEVICE_ID);
  Serial.print("Heartbeat HTTP Response: ");
  Serial.println(httpBeat);
  delay(2000); // wait for DHT to power up?
  // read sensor values
  float moisture = getMoistureLevel(PIN_MOISTURE_READ);
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature(false);
  float heatIndex = dht.computeHeatIndex(temp, humidity, false);
  int count = 0;
  while (isnan(humidity) || isnan(temp) || isnan(heatIndex)) {
    Serial.println("Error while reading sensor values, retrying");
    delay(2000);
    moisture = getMoistureLevel(PIN_MOISTURE_READ);
    humidity = dht.readHumidity();
    temp = dht.readTemperature(false);
    heatIndex = dht.computeHeatIndex(temp, humidity, false);
    count++;
    // force ESP into deepsleep so heartbeat gets out in case of permanent sensor damage
    if (count > MAX_READING_RETRIES) {
      shutdown();
    }
  }
  logSensorStatus(moisture, temp, humidity, heatIndex);
  Serial.println("Sending sensor data..");
  int httpCode = submitSensorStatus(POST_URL, moisture, temp, humidity, heatIndex);
  Serial.print("Sensor data HTTP response: ");
  Serial.println(httpCode);

  shutdown();
}

void loop() {
}

void shutdown() {
  digitalWrite(PIN_DHT_POWER, LOW);
  digitalWrite(PIN_MOISTURE_POWER, LOW);
  ESP.deepSleep(SLEEP_S * 1000000);
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
  int httpCode = httpPost(url, json);
  return httpCode;
}

int submitHeartbeat(char*  url, char* id) {
  String json = "{";
  json += "\"msg\": \"Still alive\"";
  json += ", \"id\": \"";
  json += id;
  json += "\"";
  json += ", \"type\": \"heartbeat\"";
  json += "}";
  int httpCode = httpPost(url, json);
  return httpCode;
}

// send HTTP post with JSON payload
int httpPost(char* url, String payload) {
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  if (HEADER_SECRET) {
    http.addHeader("SECRET", HEADER_SECRET);
  }
  int httpCode = http.POST(payload);
  String response = http.getString();
  if (httpCode < 0) {
    Serial.print("Error: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }
  http.end();
  return httpCode;
}

// try to connect to given SSID and key, loop until successful
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
