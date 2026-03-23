#include <Adafruit_NeoPixel.h>

#define LED_PIN 6     // Data pin connected to LEDs
#define NUM_LEDS 3    // Number of LEDs

Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    pixels.begin();  // Initialize NeoPixel library
    pixels.show();   // Turn off all LEDs at start
}

void loop() {
    for(int i = 0; i < NUM_LEDS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Red color
        pixels.show();
        delay(250);  // Wait half a second
        pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Turn off LED
        pixels.show();
    }
}