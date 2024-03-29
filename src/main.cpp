#include <Arduino.h>
#include "functions.h"

//Comment out to disable all wifi functions
#define USE_WIFI

#if defined(USE_WIFI)
  #include "wifi_setup.h"
#endif

static const char AUX_AppPage[] PROGMEM = R"(
{
  "title": "Funksender",
  "uri": "/",
  "menu": true,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "<h2>CO2-Sensor</h2>",
      "style": "text-align:center;color:#2f4f4f;padding:10px;"
    },
    {
      "name": "content",
      "type": "ACText",
      "value": "CO2-sensor und DHT22 auf Nodemcu mit ESP8266."
    }
  ]
}
)";

unsigned long timer_output_0 = 0;
unsigned long timer_output_5 = 5000;

int   mh_co2 = 0; // CO2-values MH-Z19B
float mh_t   = 0; // Temperature MH-Z19B
float dht_h  = 0; // humidity DHT 22
float dht_t  = 0; // Temperature DHT 22

void setup() {
  delay(1000);

  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  init_display();
  init_MHZ19B();
  init_DHT();
  init_neopixel(0,0,125);

  #if defined(USE_WIFI)
    Serial.println("-WiFi-");

    // Prepare the ESP8266HTTPUpdateServer
    // The /update handler will be registered during this function.
    httpUpdater.setup(&httpServer, USERNAME, PASSWORD);

    // Load a custom web page for a sketch and a dummy page for the updater.
    hello.load(AUX_AppPage);
    portal.join({ hello, update });

    if (portal.begin()) {
      if (MDNS.begin(host)) {
          MDNS.addService("http", "tcp", HTTP_PORT);
          Serial.println(" WiFi connected!");
          Serial.printf( " HTTPUpdateServer ready: http://%s.local/update\n", host);
          Serial.println();

          // MQTT Setup
          Serial.println("-MQTT-");
          client.setServer(mqtt_server, 1883);
          //client.setCallback(callback);
          reconnect();
          //snprintf (msgMQTT, 75, "G||");
          //Serial.println("Publish message [" + String(MQTT_SEND_EVENT) + "]: " + String(msgMQTT));
          //client.publish(MQTT_SEND_EVENT, msgMQTT);
          Serial.println();
      }
      else
        Serial.println("Error setting up MDNS responder");
    }
  #endif
}

void loop() {
  digitalWrite(LED, HIGH);

  #if defined(USE_WIFI)
    // Invokes mDNS::update and AutoConnect::handleClient() for the menu processing.
    mDNSUpdate(MDNS);
    portal.handleClient();
  #endif

  // Timer und Intervalllängen für die PWM-Messung des CO2-Werts
  unsigned long timer;

  timer = millis();
  if ((timer / 1000) % 10 == 0 && timer > timer_output_0) {
    timer_output_0 += 5*60*1000;  // nächste Anzeige in 5 Minuten

    mh_co2 = mhz19b.getCO2();
    if(mhz19b.errorCode != RESULT_OK) {
      delay(500);
      mh_co2 = mhz19b.getCO2();
    }
    mh_t = mhz19b.getTemperature( true );

    sensors_event_t event;
    dht.temperature().getEvent(&event);
    dht_t = event.temperature;

    dht.humidity().getEvent(&event);
    dht_h = event.relative_humidity;

    print_sensor_data( mh_co2, mh_t, mhz19b.errorCode, dht_t, dht_h );
    display_sensor_data( mh_co2, mh_t, mhz19b.errorCode, dht_t, dht_h );

    setWarningColorLevel( mh_co2 );
    //colorWipe(strip.Color(255, 0, 0), 50); // Red
    //colorWipe(strip.Color(0, 255, 0), 50); // Green
    //colorWipe(strip.Color(0, 0, 255), 50); // Blue

    #if defined(USE_WIFI)
      snprintf (msgMQTT, 75, "S|%ld|%2d|%3d", (int)((dht_h*100)+.5), (int)((dht_t*100)+.5), (int)(mh_co2)); //cast float to int and generate answer
      Serial.println("Publish message [" + String(MQTT_SEND_EVENT) + "]: " + String(msgMQTT));
      client.publish(MQTT_SEND_EVENT, msgMQTT);
      Serial.println("--------------------");
    #endif
  }
  digitalWrite(LED, HIGH);

  delay(100);  // kurz warten (1/10 Sekunde; kann entfallen)
}