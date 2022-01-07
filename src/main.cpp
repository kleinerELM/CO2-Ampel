#include <Arduino.h>
// CO2 sensor
#include <MHZ19.h>
#include <SoftwareSerial.h>
// display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// DHT22
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
//pixel
#include <Adafruit_NeoPixel.h>

#define NODE_MCU_D0 16
#define NODE_MCU_D1 5   // SCL - Display
#define NODE_MCU_D2 4   // SDA - Display
#define NODE_MCU_D3 0   
#define NODE_MCU_D4 2   
#define NODE_MCU_D5 14  // Tx - MH-Z19B
#define NODE_MCU_D6 12  // Rx - MH-Z19B
#define NODE_MCU_D7 13
#define NODE_MCU_D8 15

#define SCREEN_WIDTH    128 // OLED display width, in pixels
#define SCREEN_HEIGHT    64 // OLED display height, in pixels
#define OLED_RESET       -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LED NODE_MCU_D4

#define MHZ19_BAUDRATE 9600
MHZ19 mhz19b;  // Sensor-Objekt
SoftwareSerial co2Serial(NODE_MCU_D6, NODE_MCU_D7);

DHT_Unified dht(NODE_MCU_D5, DHT22);

uint32_t delayMS;

#define PIXEL_CNT 9
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_CNT, NODE_MCU_D3, NEO_GRB + NEO_KHZ400);

unsigned long timer_output_0 = 0;
unsigned long timer_output_5 = 5000;

int co2_ser = 0; // CO2-Werte
float temp_mh = 0; // Temperatur des MH-Z19B

void setup() {
  int i;
  char mhz19_version[4];  

  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  
  delay(100);

  Serial.println();
  
  // init display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();

  delay(500);
  co2Serial.begin(MHZ19_BAUDRATE);
  mhz19b.begin(co2Serial);  // MH-Z19B-Sensor eine Schnittstelle zuweisen
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

  strip.begin();
  strip.setBrightness(30);
  uint32_t c;
  c = strip.Color(0,0,125);
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
  }
  strip.show(); // Initialize all pixels to 'off'
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void setWarningColoerLevel(uint16 co2level) {
  uint16 pos;
  uint32_t c;
  if ( co2level < 800 ) {
    c = strip.Color(0, 255, 0);
  } else if ( co2level < 2000 ) {
    pos = ( ((float) co2level - 800.0) / 1200.0 ) * 255.0;
    c = strip.Color(pos, 255-pos, 0);
  } else {
    c = strip.Color(pos,0, 0);
  }

  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
  }
}

void loop() {
  digitalWrite(LED, HIGH);

  // Timer und Intervalllängen für die PWM-Messung des CO2-Werts
  unsigned long timer;
 
  timer = millis();
  if ((timer / 1000) % 10 == 0 && timer > timer_output_0) {
    timer_output_0 += 5000;  // nächste Anzeige in 10 Sekunden

    co2_ser = mhz19b.getCO2();

    if(mhz19b.errorCode != RESULT_OK) {
      delay(500);  // kurz warten (1/10 Sekunde; kann entfallen)
      co2_ser = mhz19b.getCO2();
    }

    if(mhz19b.errorCode != RESULT_OK) {
      Serial.println("Failed to recieve CO2 value");
      Serial.print("Response Code: ");
      Serial.println(mhz19b.errorCode);          // Get the Error Code value
    } else {
      Serial.print(F("CO2: "));
      Serial.println(co2_ser);
    }

    //delay(500);  // kurz warten (1/10 Sekunde; kann entfallen)
    temp_mh = mhz19b.getTemperature( true );

    if(mhz19b.errorCode != RESULT_OK) {
      Serial.println("Failed to recieve temperature value");
      Serial.print("Response Code: ");
      Serial.println(mhz19b.errorCode);          // Get the Error Code value
    } else {
      Serial.print(F("Temp: "));
      Serial.println( String(temp_mh, 3) );
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(5, 0);
    display.println(F("MH-Z19B-Sensordaten"));
    display.println();
    display.print(F("CO2:  "));
    display.print(co2_ser);
    display.println(F(" ppm"));
    

    Serial.println("--------------------");


    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println(F("Error reading temperature!"));
      display.print(F("Temp: "));
      display.print(String(temp_mh, 0));
      display.write(247);
      display.println(F("C"));
    }
    else {
      Serial.print(F("Temperature: "));
      Serial.print(event.temperature);
      Serial.println(F("°C"));

      display.print(F("Temp: "));
      display.print(String(event.temperature, 1));
      display.write(247);
      display.print(F("C ("));
      display.print(String(temp_mh, 0));
      display.write(247);
      display.println(F("C)"));
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("Error reading humidity!"));
    }
    else {
      Serial.print(F("Humidity: "));
      Serial.print(event.relative_humidity);
      Serial.println(F("%"));

      display.print(F("RH:   "));
      display.print(String(event.relative_humidity, 1));
      display.println(F(" %"));
    }

    display.display();      // Show initial text

    setWarningColoerLevel(co2_ser);
    //colorWipe(strip.Color(255, 0, 0), 50); // Red
    //colorWipe(strip.Color(0, 255, 0), 50); // Green
    //colorWipe(strip.Color(0, 0, 255), 50); // Blue


  }
  digitalWrite(LED, HIGH);

  delay(100);  // kurz warten (1/10 Sekunde; kann entfallen)
}