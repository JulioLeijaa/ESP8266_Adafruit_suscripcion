#include <ESP8266WiFi.h>
#include <WiFiClient.h>             //D0---bomba(IN1 RELAY)
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define WLAN_SSID "-----" 
#define WLAN_PASS "-----" 
/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  "----"
#define AIO_KEY       "----"
/************ Global State  ******************/
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
/****************************** Feeds ***************************************/
// Setup a feed called 'potValue' for publishing.
Adafruit_MQTT_Publish pbomba = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/bomba",MQTT_QOS_1);
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
// Setup a feed called 'ledBrightness' for subscribing to changes.
Adafruit_MQTT_Subscribe sbomba = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/bomba",MQTT_QOS_1);
/*************************** Sketch Code ************************************/
// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();
int bomba = 16;

void digitalCallback(char *data, uint16_t len) {
  Serial.print("Bomba value is: ");
  Serial.println(data);

     String message = String(data);
      message.trim();
      if (message == "ON") {
        digitalWrite(bomba, LOW); 
        delay(30000);
        digitalWrite(bomba, HIGH);
         pbomba.publish("OFF");
         }
      if (message == "OFF") {
        digitalWrite(bomba, HIGH);} 
}

void setup() {
  Serial.begin(115200);
 
  pinMode(bomba, OUTPUT);digitalWrite(bomba,HIGH);
  Serial.println();
  Serial.print("Connecting to ");Serial.println(WLAN_SSID); 
  WiFi.encryptionType(ENC_TYPE_CCMP); WiFi.begin(WLAN_SSID, WLAN_PASS); WiFi.printDiag(Serial);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println(); Serial.println("WiFi connected");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());

  sbomba.setCallback(digitalCallback);
  mqtt.subscribe(&sbomba);
  }

void loop() {
  MQTT_connect();
  
  mqtt.processPackets(500);
  delay(10000);
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return; // Stop if already connected.
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
