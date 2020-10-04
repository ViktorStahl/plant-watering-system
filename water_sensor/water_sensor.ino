/*
  ESP8266 Blink by Simon Peter
  Blink the blue LED on the ESP-01 module
  This example code is in the public domain

  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)

  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/

/* **** Includes **** */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// The config.h is needed to set wifi credentials and mqtt broker IP.
#include "config.h"

/* **** Defines **** */

#define MOISTURE_SENSOR     (0u)
#define AIR                 (850)
#define WATER               (450)
#define EARTH_SEMI_MOIST    (600)

#define MAC_LEN             (6u)
#define TOPIC_MAC_OFFSET    (5u)

/* **** Type Definitions **** */

/* **** Global Variables **** */

/* **** Static Variables **** */

static byte mac[MAC_LEN];
static char moisture_topic[] = "home/00:00:00:00:00:00/moisture";

WiFiClient espClient;
PubSubClient client(espClient);

int readMoisture() {
  int val = analogRead(MOISTURE_SENSOR); //connect sensor to Analog 0
  int moisture = 100 - ((val - WATER) * 100) / (AIR-WATER);
  Serial.printf("Moisture %u%%\n", moisture); // print the value to serial port
  return moisture;
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

void printMAC(const byte* mac) {
  for (int i = 0; i < MAC_LEN; i++)
  {
    Serial.print(mac[i], HEX);
    if (i != MAC_LEN - 1)
    {
      Serial.print(":");
    }
  }
}

static void setup_wifi() {
  delay(10);
  Serial.print("\nConnecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi OK ");
  Serial.print("=> ESP8266 IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println();
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMAC(mac);
  Serial.println();
}

static void reconnect() {

  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ESP8266Client")) {
      Serial.println("OK");
    } else {
      Serial.print("Error : ");
      Serial.print(client.state());
      Serial.println(" Wait 5 secondes before to retry");
      delay(5000);
    }
  }
}

static void setup_mqtt_topic(char* topicStr, const byte* macAdr) {
  char macPart[3];
  for (int i = 0; i < MAC_LEN; i++)
  {
    sprintf(macPart, "%02X", mac[i]);
    int topicIdx = TOPIC_MAC_OFFSET + i * 3;
    memcpy((void*)&topicStr[topicIdx], macPart, (2 * sizeof(char)));
  }
}

static void sendMoisture(int moisture) {
  client.publish(moisture_topic, String(moisture).c_str(), true);
}

static void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("In callback");
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(9600);
  Serial.println("\nI am awake!");
  setup_wifi();           //Connect to Wifi network
  setup_mqtt_topic(moisture_topic, mac);       // Set unique MQTT topic 
  client.setServer(mqtt_server, mqtt_port);    // Configure MQTT connexion
  client.setCallback(callback);           // callback function to execute when a MQTT message
}

// the loop function runs over and over again forever
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
                                    // but actually the LED is on; this is because
                                    // it is active low on the ESP-01)
  delay(1000);                      // Wait for a second
  int moisture = readMoisture();
  sendMoisture(moisture);
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH

  delay(300000);                      // Wait for five minutes (to demonstrate the active low LED)
  // runTest7();
}
