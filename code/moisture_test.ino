// One nail is connected to +3.2V (i.e., pin 13), and the other nail is connected to this analog pin.
// Since AO on the Feather Huzzah only reads from 0-1V, we also use a voltage divider
// to drop the voltage from the range 0-3.2V down to 0-1V.
uint8_t soilPin = 0;   // Define the analog pin number for the soil moisture sensor as 0
int moisture_val;      // Variable to store the moisture value read from the sensor

int sensorVCC = 13;    // Define the power supply pin as digital pin 13
int counter = 0;       // Counter for controlling the measurement interval

void setup() {
  Serial.begin(115200);     // Start serial communication at 115200 baud rate
  pinMode(sensorVCC, OUTPUT); // Set the sensorVCC pin as an output
  digitalWrite(sensorVCC, LOW); // Initially turn off the power to the sensor
}

void loop() {
  counter++;  // Increment the counter value
  // If the counter is greater than 6, activate the sensor (change this value to set the "not powered" time, larger number means longer gap)
  if(counter > 6){
    // Power on the sensor
    digitalWrite(sensorVCC, HIGH);
    delay(1000); // Wait some time for the sensor to stabilize
    // Read the value from the sensor:
    moisture_val = analogRead(soilPin);   // Read the resistance value from the sensor
    // Power off the sensor
    digitalWrite(sensorVCC, LOW);
    delay(100);
    
    // Output the sensor reading through the serial port
    Serial.print("sensor = ");                       
    Serial.println(moisture_val);     
    counter = 0;    // Reset the counter
  }  
  // Wait
  Serial.print(".");                       
  delay(1000);
}
