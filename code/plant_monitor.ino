#include <ESP8266WiFi.h>               // Include ESP8266 WiFi library
#include <ESP8266WebServer.h>           // Include the ESP8266 WebServer library
#include <ezTime.h>                  // Include the ezTime library for handling date and time
#include <PubSubClient.h>             // Include the MQTT PubSubClient library
#include <DHT.h>                      // Include the DHT sensor library
#include <DHT_U.h>                      // Include the DHT Unified sensor library

#define DHTTYPE DHT22                // Define the DHT sensor type. Here it is DHT 22 (AM2302, AM2321)
const int dryValue = 800; // Sensor readings when completely dry
const int wetValue = 300; // Sensor readings when completely wet

// Sensors - DHT22 and Soil Moisture Sensor
uint8_t DHTPin = 12;                   // DHT sensor is connected to Pin 12 of the Huzzah ESP8266
uint8_t soilPin = 0;                   // Soil Moisture Sensor is connected to the ADC or A0 pin on Huzzah
float Temperature;                     // Variable to store temperature
float Humidity;                         // Variable to store humidity
int Moisture = 1;                       // Initial moisture value (before first read)
int sensorVCC = 13;                    // Power pin for the Soil Moisture Sensor
int blueLED = 2;                        // Blue LED pin for status indication
DHT dht(DHTPin, DHTTYPE);             // Initialize DHT sensor for the temperature and humidity

// Wifi and MQTT Credentials
#include "arduino_secrets.h"            // Include a file for secret credentials like WiFi and MQTT credentials
/* 
**** Please enter your sensitive data in the Secret tab/arduino_secrets.h
**** using the format below:

#define SECRET_SSID "ssid name"
#define SECRET_PASS "ssid password"
#define SECRET_MQTTUSER "user name - eg student"
#define SECRET_MQTTPASS "password";
 */

const char* ssid     = SECRET_SSID;     // Set WiFi SSID
const char* password = SECRET_PASS;     // Set WiFi password
const char* mqttuser = SECRET_MQTTUSER; // Set MQTT user
const char* mqttpass = SECRET_MQTTPASS; // Set MQTT password

ESP8266WebServer server(80);          // Create a web server on port 80
const char* mqtt_server = "mqtt.cetools.org"; // MQTT server address
WiFiClient espClient;                 // Create a WiFiClient object for the network connection
PubSubClient client(espClient);           // Create a PubSubClient client for MQTT communication
long lastMsg = 0;                       // Last time a message was sent
char msg[50];                           // Buffer to store the message string
int value = 0;                          // Variable to store data to be sent

// Date and time
Timezone GB;                            // Create a Timezone object for Great Britain

void setup() {
  // Set up and initialize hardware and network connections
  pinMode(BUILTIN_LED, OUTPUT);         // Set the built-in LED pin as an output
  digitalWrite(BUILTIN_LED, HIGH);      // Turn off the LED (ESP8266's built-in LED is active low)

  // Setup for the soil moisture sensor and blue LED
  pinMode(sensorVCC, OUTPUT);           // Set the sensor power pin as output
  digitalWrite(sensorVCC, LOW);         // Turn off power to the sensor initially
  pinMode(blueLED, OUTPUT);             // Set the blue LED pin as output
  digitalWrite(blueLED, HIGH);          // Turn off the blue LED (assuming active low)

  Serial.begin(115200);                 // Start serial communication at 115200 baud for debugging
  delay(100);                           // Wait a bit for serial communication to stabilize

  dht.begin();                          // Initialize the DHT sensor

  // Run initialization functions for WiFi, web server, and time sync
  startWifi();                          // Connect to WiFi
  startWebserver();                     // Start the web server
  syncDate();                           // Synchronize date and time

  client.setServer(mqtt_server, 1884);  // Set MQTT server and port
  client.setCallback(callback);         // Set the callback function for MQTT messages

  // Add your remaining setup code here
}

void loop() {
  // Main program loop
  server.handleClient();                // Handle client requests to the web server

  if (minuteChanged()) {                // Check if the minute has changed to perform actions every minute
    readMoisture();                     // Read moisture levels from the sensor
    sendMQTT();                         // Send data over MQTT
    Serial.println(GB.dateTime("H:i:s")); // Print the current time in Great Britain timezone
  }

void readMoisture(){
  // power the sensor
  digitalWrite(sensorVCC, HIGH);       // Turn on the power to the moisture sensor
  digitalWrite(blueLED, LOW);          // Turn off the blue LED
  delay(100);                          // Wait for 100 milliseconds
  // read the value from the sensor:
  Moisture = analogRead(soilPin);      // Read the moisture level from the soil moisture sensor
  MoisturePercent = map(Moisture, wetValue, dryValue, 100, 0);
  MoisturePercent = constrain(MoisturePercent, 0, 100);

  digitalWrite(sensorVCC, LOW);        // Turn off the power to the moisture sensor
  digitalWrite(blueLED, HIGH);         // Turn on the blue LED
  delay(100);                          // Wait for 100 milliseconds
  Serial.print("Wet ");                
  Serial.println(Moisture);            // Print the moisture level to the serial monitor
  Serial.print(" Moisture (%): ");
  Serial.println(MoisturePercent);
}

void startWifi() {
  // We start by connecting to a WiFi network
  Serial.println();                    
  Serial.print("Connecting to ");      
  Serial.println(ssid);                // Print the SSID that we are trying to connect to
  WiFi.begin(ssid, password);          // Begin connecting to the WiFi network

  // check to see if connected and wait until you are
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);                        // Wait for 500 milliseconds before checking again
    Serial.print(".");                 // Print a dot on the serial monitor as a progress indicator
  }
  Serial.println("");
  Serial.println("WiFi connected");    // Notify on serial monitor that WiFi is connected
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());      // Print the IP address assigned to the device
}

void syncDate() {
  // get real date and time
  waitForSync();                       // Wait for time synchronization
  Serial.println("UTC: " + UTC.dateTime()); // Print the UTC date and time
  GB.setLocation("Europe/London");     // Set the location for time conversion
  Serial.println("London time: " + GB.dateTime()); // Print the local time in London
}

void sendMQTT() {

  if (!client.connected()) {
    reconnect();                       // Reconnect to MQTT if not already connected
  }
  client.loop();                       // Maintain MQTT connection

  Temperature = dht.readTemperature(); // Gets the values of the temperature
  snprintf (msg, 50, "%.1f", Temperature); // Format the temperature reading into a string
  Serial.print("Publish message for t: ");
  Serial.println(msg);                 // Print the temperature message to be published
  client.publish("student/CASA0014/plant/ucfnll0/temperature", msg); // Publish the temperature message

  Humidity = dht.readHumidity();       // Gets the values of the humidity
  snprintf (msg, 50, "%.0f", Humidity); // Format the humidity reading into a string
  Serial.print("Publish message for h: ");
  Serial.println(msg);                 // Print the humidity message to be published
  client.publish("student/CASA0014/plant/ucfnll0/humidity", msg); // Publish the humidity message

  //Moisture = analogRead(soilPin);   // moisture read by readMoisture function
  snprintf (msg, 50, "%.0i", Moisture); // Format the moisture reading into a string
  Serial.print("Publish message for m: ");
  Serial.println(msg);                 // Print the moisture message to be published
  client.publish("student/CASA0014/plant/ucfnll0/moisture", msg); // Publish the moisture message
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);                 // Print the topic of the received MQTT message
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);    // Print each character of the payload
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);    // turn the LED on (Note that LOW is the voltage level but actually the LED is on; this is because it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);   // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX); // Generate a random client ID
    
    // Attempt to connect with clientID, username and password
    if (client.connect(clientId.c_str(), mqttuser, mqttpass)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("student/CASA0014/plant/ucfnll0/inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());     // print the return code of the failed connection attempt
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds 
      delay(5000);
    }
  }
}

void startWebserver() {
  // when connected and IP address obtained start HTTP server  
  server.on("/", handle_OnConnect);    // Define the handler for the root path
  server.onNotFound(handle_NotFound);  // Define the handler for not found URLs
  server.begin();                      // Start the HTTP server
  Serial.println("HTTP server started");  
}

void handle_OnConnect() {
  Temperature = dht.readTemperature(); // Get the values of the temperature
  Humidity = dht.readHumidity();       // Get the values of the humidity
  server.send(200, "text/html", SendHTML(Temperature, Humidity, Moisture)); // Send the HTML content
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found"); // Send a 404 not found message
}

String SendHTML(float Temperaturestat, float Humiditystat, int Moisturestat) {
  // Construct HTML content to send over the server
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP8266 DHT22 Report</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>ESP8266 DHT22 Sensor</h1>\n";
  ptr += "<p>Temperature: ";
  ptr += (int)Temperaturestat;
  ptr += " *C</p>";
  ptr += "<p>Humidity: ";
  ptr += (int)Humiditystat;
  ptr += " %</p>";
  ptr += "<p>Moisture: ";
  ptr += Moisturestat;
  ptr += "</p>";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
