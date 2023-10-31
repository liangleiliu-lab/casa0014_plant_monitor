// Include the ESP8266WiFi library to control the ESP8266 WiFi module
#include <ESP8266WiFi.h>

// Define the SSID and Password of the WiFi network 
const char* ssid     = "xxx";
const char* password = "xxx";
// Define the hostname of the server
const char* host = "iot.io";

// Setup function
void setup() {
  // Start serial communication with a baud rate of 115200
  Serial.begin(115200);
  // Delay for 100 milliseconds to ensure stability
  delay(100);

  // Begin the process of connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // Initialize WiFi connection
  WiFi.begin(ssid, password);
  
  // Loop until the WiFi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print that WiFi is connected and the assigned IP address
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Define a global variable 'value' for counting (not used in this example)
int value = 0;

// Main loop function
void loop() {
  // Delay for 5000 milliseconds at the start of each loop
  delay(5000);
  ++value; // Increment the value of 'value'

  Serial.println("-------------------------------");
  Serial.print("Connecting to ");
  Serial.println(host);
  
  // Use the WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  // Attempt to connect to the server's HTTP port (80)
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // Create the URL for the HTTP GET request
  String url = "/index.html";
  Serial.print("Requesting URL: ");
  Serial.println(host + url);
  
  // Send the HTTP GET request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  
  // Wait for 500 milliseconds to receive the response from the server
  delay(500);

  // Read and print the server response to the serial port
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  // Delay for 1000000 milliseconds before executing the loop again
  delay(1000000);
}
