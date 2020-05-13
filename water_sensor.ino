/*
  ESP8266 Blink by Simon Peter
  Blink the blue LED on the ESP-01 module
  This example code is in the public domain

  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)

  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/

#define MOISTURE_SENSOR     (0u)
#define AIR                 (850)
#define WATER               (450)
#define EARTH_SEMI_MOIST    (600)

void readMoisture() {
  int val = analogRead(MOISTURE_SENSOR); //connect sensor to Analog 0
  int moisture = 100 - ((val - WATER) * 100) / (AIR-WATER);
  Serial.printf("Moisture %u%%\n", moisture); // print the value to serial port
}

void readVoltage() { // read internal VCC
  float volts = ESP.getVcc();
  Serial.printf("The internal VCC reads %1.2f volts\n", volts / 1000);
}

void runTest7() {
  readVoltage();                                       // read internal VCC
  digitalWrite(LED_BUILTIN, LOW);                      // turn the LED on, at least briefly
  Serial.println(F("going into Deep Sleep now..."));
  ESP.deepSleep(10E6, WAKE_RF_DEFAULT);                // good night!  D0 fires a reset in 10 seconds...
  Serial.println(F("What... I'm not asleep?!?"));      // it will never get here
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(9600);
  Serial.println("\nI am awake!");
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
                                    // but actually the LED is on; this is because
                                    // it is active low on the ESP-01)
  delay(1000);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  readMoisture();
  delay(2000);                      // Wait for two seconds (to demonstrate the active low LED)
  // runTest7();
}
