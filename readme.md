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
