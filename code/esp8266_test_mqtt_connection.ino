#include <ESP8266WiFi.h>  //Includes the ESP8266WiFi library for WiFi functionalities
#include <PubSubClient.h>  //Includes the ESP8266WiFi library for MQTT functionalities

//include configuration of  Wifi and MQTT
#include "arduino_secrets.h" 
/*
WiFi and MQTT server details are fetched from the arduino_secrets.h.
*/
const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser =SECRET_MQTTUSER;
const char* mqttpass=SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";

WiFiClient espClient; //creates a client for wifi
PubSubClient client(espClient); //initializes MQTT client with wifi client
long lastMsg = 0; //variable to keep track of the last message
char msg[50]; //message buffer
int value = 0;  //value to send in the message

void setup() {
  
  // We setup an LED to be controllable via MQTT
  // Initialize the BUILTIN_LED pin as an output 
  // Turn the LED off by making the voltage HIGH
  pinMode(BUILTIN_LED, OUTPUT);     
  digitalWrite(BUILTIN_LED, HIGH);  

  // open serial connection via the wifi to the mqtt broker
  Serial.begin(115200);
  delay(100); // to give time for the serial connection to open

  // Initiate the connecting to wifi routine
  startWifi();

  // Once connected to wifi establish connection to mqtt broker
  client.setServer(mqtt_server, 1884);
  
  // The callback in this case listens for instructions to 
  // change the state of the LED - here we are initialising 
  // that function
  client.setCallback(callback);

}

void loop() {
  delay(5000); //wait for 5 seconds
  sendMQTT(); //send a massage to mqtt server
}


// This function is used to set-up the connection to the wifi
// using the user and passwords defined in the secrets file
// It then prints the connection status to the serial monitor

void startWifi(){
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // check to see if connected and wait until you are
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); //waits while connecting
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // display the IP address once connected.
}

// This function is used to make sure the arduino is connected
// to an MQTT broker before it tries to send a message and to 
// keep alive subscriptions on the broker (ie listens for inTopic)

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {    // while not (!) connected....
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqttuser, mqttpass)) {
      Serial.println("connected");
      // ... and subscribe to messages on broker
      client.subscribe("student/CASA0014/plant/ucfnll0/inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// This function sends (publishes) a message to a MQTT topic
// once a connection is established with the broker. It sends
// an incrementing variable called value to the topic:
// "student/CASA0014/plant/ucjtdjw"

void sendMQTT() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ++value;
  snprintf (msg, 50, "hello world #%ld", value);
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfnll0", msg);

}

// The callback function is called when an incoming message is received
// from the MQTT broker (ie the inTopic message)/ In this demo if the first
// character of the message has the value "1" we turn an LED on. Any other value 
// results in the LED being turned off

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

    // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}
