# plant buddy

> nodeMCU based moisture monitoring for plants 🌱 with a serverless backend

## Hardware

The hardware setup consists of a [ESP8266](https://en.wikipedia.org/wiki/ESP8266) Microcontroller with a powersupply and two connected sensors.

- [DHT222](https://www.adafruit.com/product/385) connected to **D2** digital input pin
- [Soil moisture sensor](https://www.sparkfun.com/products/13322) connected to **A0**
- Bridge between D0 and RST to enable deep sleep for optimizing battery life

[<img src="sketch.png" height="600px" alt="Fritzing diagram"/>](sketch.png)

## Useful links

- https://openhomeautomation.net/esp8266-battery
- http://www.instructables.com/id/Interface-Moisture-Sensor-With-NodeMCU/
- https://www.losant.com/blog/getting-started-with-the-esp8266-and-dht22-sensor
- https://www.losant.com/blog/making-the-esp8266-low-powered-with-deep-sleep