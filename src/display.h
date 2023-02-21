// written by Florian Kleiner 2023

// display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH    128 // OLED display width, in pixels
#define SCREEN_HEIGHT    64 // OLED display height, in pixels
#define OLED_RESET       -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void init_display(){
	delay(100);

	Serial.println();

	// init display
	if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
		Serial.println(F("SSD1306 allocation failed"));
		for(;;); // Don't proceed, loop forever
	}

	display.display();

	delay(500);
}

void display_sensor_data( uint16_t mh_co2, float mh_t, byte mh_err, float dht_t, float dht_h ) {
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(SSD1306_WHITE);
	display.setCursor(5, 0);
	if (mh_err == 1) { //RESULT_OK == 1
		display.println(F("MH-Z19B-Sensordaten"));
		display.println();
		display.print(F("CO2:  "));
		display.print(mh_co2);
		display.println(F(" ppm"));

		if (isnan(dht_t)) {
			display.print(F("Temp: "));
			display.print(String(mh_t, 0));
			display.write(247);
			display.println(F("C"));
		} else {
			display.print(F("Temp: "));
			display.print(String(dht_t, 1));
			display.write(247);
			display.print(F("C ("));
			display.print(String(mh_t, 0));
			display.write(247);
			display.println(F("C)"));
		}

		if (!isnan(dht_h)) {
			display.print(F("RH:   "));
			display.print(String(dht_t, 1));
			display.println(F(" %"));
		}
	} else {
		display.println(F("MH-Z19B-Sensorfehler"));
		display.println();
		display.println(F("konnte CO2-Daten"));
		display.println(F("nicht auslesen"));
	}
	display.display();      // Show text
}