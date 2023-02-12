// written by Florian Kleiner 2023

// include basic libaries

#if defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266mDNS.h>
  #include <ESP8266HTTPUpdateServer.h>
  #define HOSTIDENTIFY  "esp8266"
  #define mDNSUpdate(c)  do { c.update(); } while(0)
  using WebServerClass = ESP8266WebServer;
  using HTTPUpdateServerClass = ESP8266HTTPUpdateServer;
#elif defined(ARDUINO_ARCH_ESP32)
  #include <WiFi.h>
  #include <WebServer.h>
  #include <ESPmDNS.h>
  #include "HTTPUpdateServer.h"
  #define HOSTIDENTIFY  "esp32"
  #define mDNSUpdate(c)  do {} while(0)
  using WebServerClass = WebServer;
  using HTTPUpdateServerClass = HTTPUpdateServer;
#endif
#include <WiFiClient.h>
#include <AutoConnect.h>

#include <PubSubClient.h>

WiFiClient espWiFi;
PubSubClient client(espWiFi); //MQTT

// Fix hostname for mDNS. It is a requirement for the lightweight update feature.
static const char* host = HOSTIDENTIFY "-webupdate";
#define HTTP_PORT 80

// ESP8266WebServer instance will be shared both AutoConnect and UpdateServer.
WebServerClass  httpServer(HTTP_PORT);

#define USERNAME "user"   //*< Replace the actual username you want */
#define PASSWORD "pass"   //*< Replace the actual password you want */

// autoconnect AP-passwd = 12345678

// Declare AutoConnectAux to bind the HTTPWebUpdateServer via /update url
// and call it from the menu.
// The custom web page is an empty page that does not contain AutoConnectElements.
// Its content will be emitted by ESP8266HTTPUpdateServer.
HTTPUpdateServerClass httpUpdater;
AutoConnectAux  update("/update", "Update");

// Declare AutoConnect and the custom web pages for an application sketch.
AutoConnect     portal(httpServer);
AutoConnectAux  hello;

//MQTT
#define MQTT_DEVICE "CO2_nodemcu8266"
#define MQTT_SEND_EVENT "CO2_result"
const char* mqtt_server = "192.168.2.52";
char msgMQTT[200];

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect( MQTT_DEVICE )) {
      Serial.println(" connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}