# casa0017 plant monitor 
## overview
The goal of this project is to build a sensor system that can sense air temperature and humidity and soil moisture. The data will be transmitted to the mqtt server via WiFi. The data is also stored in IfluxDB deployed on a Raspberry Pi, and we visualise the data using Telegraf, and Grafana, also deployed on a Raspberry Pi.
## Method
### Incremental build 
This project uses an incremental development model. Different components are first developed and tested independently, and then gradually integrated into a system.
Therefore the following will follow the actual development process.
## Developing process
### Feather Huzzah ESP8266 SETUP
Start the arduino IDE and plug the Huzzah ESP8266 into the USB port. 
For WIN11 system, you need to install [CP2104 driver](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide) in advance for the first time to use this development board, so that arduino IDE will not recognise it.
**WIFI connection** 
This development board has a WiFi chip on board, which means we can connect directly to the internet. Firstly we need to test the WiFi functionality of this development board. The script for this test can be found [here](https://github.com/liangleiliu-lab/casa0017_plant_monitor/blob/main/code/esp8266_wifi_connection_setup.ino). Here we need to include the ESP8266 WiFi library to make it easier for us to access the internet.

In the script, we need to add the name and password of the WiFi we want to connect to.
```C++
#include <ESP8266WiFi.h>

const char* ssid     = "SSID here";
const char* password = "password here";
```
Also we need to turn on the serial monitor so that we can print out debugging information.

### Feather Huzzah ESP8266 send data to MQTT server
This part is designed to connect an ESP8266 microcontroller to a Wi-Fi network and an MQTT (Message Queuing Telemetry Transport) server for publishing and subscribing to messages. The full test code can be found [here](https://github.com/liangleiliu-lab/casa0017_plant_monitor/blob/main/code/esp8266_test_mqtt_connection.ino). 

We used the [PubSubClient](https://pubsubclient.knolleary.net/) library to do many of the MQTT connection setup.Here we test using the MQTT server provided by the lab at `mqtt.cetools.org`.For server security, we use a separate file [arduino_secrets.h](https://github.com/liangleiliu-lab/casa0017_plant_monitor/blob/main/arduino_sercert.h) to store WiFi and MQTT-related credentials.
```c
#define SECRET_SSID "xxx"
#define SECRET_PASS "xxx"
#define SECRET_MQTTUSER "xxx"
#define SECRET_MQTTPASS "xxx"
```
In this side of the script we send a message to the MQTT server and subscribe to the message at the same time, if the first bit of the payload of the message we subscribe to is 1, the led light of the development board will light up, otherwise it will not light up.

### Detection of air temperature and humidity and soil moisture
For air temperature and humidity, we use a DHT22 sensor for measurement.
![DTH22](assets/844482b079b413dc846d1e51013ca72.jpg)
<div align="center">
  DTH22
</div>

The code for the testing of this temperature and humidity sensor can be found [here](https://github.com/liangleiliu-lab/casa0017_plant_monitor/blob/main/code/DHT22_TEST.ino), again we used a library written specifically for the DHT22.

For soil moisture detection, we used nail-type moisture sensors, also known as soil moisture probes or spikes, are tools used to measure the water content in soil. These sensors are particularly useful in agriculture, landscaping, and environmental monitoring to ensure optimal soil moisture levels for plant growth and soil health.

**Working Principle:**
 Most nail-type sensors operate based on electrical resistance or capacitance. They measure how easily an electrical current can pass through the soil, which changes with the soil's moisture content.
Conductivity: In general, wet soil conducts electricity better than dry soil. So, the more moisture in the soil, the less resistance or the higher capacitance the sensor will detect.
Below are its schematic and physical diagrams respectively.![1](https://github.com/liangleiliu-lab/casa0017_plant_monitor/blob/main/assets/1e16100084145105fbd121196ab7a69.png)
![2](https://github.com/liangleiliu-lab/casa0017_plant_monitor/blob/main/assets/b11ade06aab599456bee00bb285c494.jpg)
The code for the testing of this nail-type moisture sensor can be found [here](https://github.com/liangleiliu-lab/casa0017_plant_monitor/blob/main/code/moisture_test.ino). The fact that the data we get from our measurements are purely resistance values, we need to plot **the corresponding calibration curves** to be able to accurately judge the moisture content of the soil.
### ESP send soil and air data to MQTT
