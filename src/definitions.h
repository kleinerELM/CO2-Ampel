// written by Florian Kleiner 2023

// include basic libaries

// CO2 sensor
#include <MHZ19.h>
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

#if defined(ARDUINO_ARCH_ESP8266)


	// basic pin definitions
	#define NODE_MCU_D0 16
	#define NODE_MCU_D1 5   // SCL - Display
	#define NODE_MCU_D2 4   // SDA - Display
	#define NODE_MCU_D3 0
	#define NODE_MCU_D4 2
	#define NODE_MCU_D5 14  // Tx - MH-Z19B
	#define NODE_MCU_D6 12  // Rx - MH-Z19B
	#define NODE_MCU_D7 13
	#define NODE_MCU_D8 15
	
	// CO2-sensor setup
	#include <SoftwareSerial.h>
	SoftwareSerial co2Serial(NODE_MCU_D6, NODE_MCU_D7);

	// screen setup, esp8266 only
	#define SCREEN_WIDTH    128 // OLED display width, in pixels
	#define SCREEN_HEIGHT    64 // OLED display height, in pixels
	#define OLED_RESET       -1 // Reset pin # (or -1 if sharing Arduino reset pin)
	#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
	Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

	// neopixel setup
	#define LED NODE_MCU_D4
	#define PIXEL_CNT 9 // neopixel led count
	Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_CNT, NODE_MCU_D3, NEO_GRB + NEO_KHZ400);

#elif defined(ARDUINO_ARCH_ESP32)

	#define NODE_MCU_D0 16
	#define NODE_MCU_D1 5   // SCL - Display
	#define NODE_MCU_D2 4   // SDA - Display
	#define NODE_MCU_D3 0
	#define NODE_MCU_D4 2
	#define NODE_MCU_D5 14  // Tx - MH-Z19B
	#define NODE_MCU_D6 12  // Rx - MH-Z19B
	#define NODE_MCU_D7 13
	#define NODE_MCU_D8 15
	#define RXCO2	  	16  // Tx - MH-Z19B
	#define TXCO2 		17  // Rx - MH-Z19B

#endif

// CO2-sensor setup
#define MHZ19_BAUDRATE 9600
MHZ19 mhz19b;  // Sensor-Objekt


DHT_Unified dht(NODE_MCU_D5, DHT22);