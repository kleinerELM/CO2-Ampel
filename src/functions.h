// written by Florian Kleiner 2023

#include "definitions.h"

// load basic functions
void init_MHZ19B() {
  int i;
  char mhz19_version[4];
  
  #if defined(ARDUINO_ARCH_ESP8266)
    co2Serial.begin(MHZ19_BAUDRATE);
    mhz19b.begin(co2Serial);  // MH-Z19B-Sensor eine Schnittstelle zuweisen
  #elif defined(ARDUINO_ARCH_ESP32)
    Serial2.begin(MHZ19_BAUDRATE, SERIAL_8N1, RXCO2, TXCO2);
    mhz19b.begin(Serial2);  // MH-Z19B-Sensor eine Schnittstelle zuweisen
  #endif

  mhz19b.getVersion(mhz19_version);
  Serial.print("--------------------\nMH-Z19B Firmware Version: ");
  for (i = 0; i < 4; i++) {
    Serial.print(mhz19_version[i]);
    if (i == 1) Serial.print(".");
  }
  Serial.print("\nMH-Z19B Messbereich: ");    Serial.println(mhz19b.getRange());
  Serial.print("MH-Z19B Autokalibrierung: "); mhz19b.getABC() ? Serial.println("AN") :  Serial.println("AUS");
  Serial.print("Background CO2: ");  Serial.println(mhz19b.getBackgroundCO2());
  Serial.print("Temperature Cal: "); Serial.println(mhz19b.getTempAdjustment());
  Serial.println(F("------------------------------------"));
}

uint32_t delayMS;
void init_DHT() {
  dht.begin();
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value);  Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value);  Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value);  Serial.println(F(" %"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value);  Serial.println(F(" %"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F(" %"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

void print_sensor_data( uint16_t mh_co2, float mh_t, byte mh_err, float dht_t, float dht_h ) {
    if(mh_err != RESULT_OK) {
      Serial.println("Failed to recieve CO2 value");
      Serial.print("Response Code: ");
      Serial.println(mh_err);          // Get the Error Code value
    } else {
      Serial.print(F("CO2: "));
      Serial.print(mh_co2);
      Serial.println(F(" ppm"));
    }

    if(mh_err != RESULT_OK) {
      Serial.println("Failed to recieve MH-Z19B temperature value");
      Serial.print("Response Code: ");
      Serial.println(mh_err);          // Get the Error Code value
    } else {
      Serial.print(F("Temperature: "));
      Serial.print( String(mh_t, 2) );
      Serial.println(F("°C"));
    }

    if (isnan(dht_t)) {
      Serial.println(F("Error reading DHT-22 temperature!"));
    } else {
      Serial.print(F("Temperature: "));
      Serial.print(dht_t);
      Serial.println(F("°C"));
    }

    if (isnan(dht_h)) {
      Serial.println(F("Error reading DHT-22 humidity!"));
    } else {
      Serial.print(F("Humidity: "));
      Serial.print(dht_h);
      Serial.println(F(" %"));
    }

    Serial.println("--------------------");
}
