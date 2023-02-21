// written by Florian Kleiner 2023

#include <Adafruit_NeoPixel.h>

#define LED NODE_MCU_D4
#define PIXEL_CNT 9 // neopixel led count
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_CNT, NODE_MCU_D3, NEO_GRB + NEO_KHZ400);

void init_neopixel(uint8_t r, uint8_t g, uint8_t b){
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
	
	strip.begin();
	strip.setBrightness(30);
	uint32_t c;
	c = strip.Color(r,g,b);
	for(uint16_t i=0; i<strip.numPixels(); i++) {
		strip.setPixelColor(i, c);
		strip.show();
	}
	strip.show(); // Initialize all pixels
	}

	// Fill the dots one after the other with a color
	void colorWipe(uint32_t c, uint8_t wait) {
	for(uint16_t i=0; i<strip.numPixels(); i++) {
		strip.setPixelColor(i, c);
		strip.show();
		delay(wait);
	}
}

void setWarningColorLevel(uint16_t co2level) {
	uint16_t pos;
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
