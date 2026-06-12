// ---------- Libraries ----------
// Install via Arduino IDE -> Tools -> Manage Libraries...
//   "Adafruit NeoPixel"
//   "Adafruit SSD1306"  (also installs "Adafruit GFX Library")
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// ---------- NeoPixel ----------
#define LED_PIN   13
#define LED_COUNT 16
// NEO_GRB and NEO_KHZ800 come from the Adafruit_NeoPixel library -- do NOT define them yourself.
Adafruit_NeoPixel ring = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Ring helper -- set all LEDs to one color
void fillRing(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < LED_COUNT; i++) ring.setPixelColor(i, ring.Color(r, g, b));
    ring.show();
}

void fillLowerSemiRing(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < 3; i++) ring.setPixelColor(i, ring.Color(r, g, b));
    for (int i = 3; i < 6; i++) ring.setPixelColor(i, ring.Color(0, 0, 0));
    for (int i = 6; i < LED_COUNT; i++) ring.setPixelColor(i, ring.Color(r, g, b));
    ring.show();
}

void expressionsBegin() {
    ring.begin();
    ring.setBrightness(255);
    ring.show();  // all LEDs off
    randomSeed(micros());
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    expressionsBegin();   // run hardware init ONCE here, not in loop()
}

void loop() {
    fillRing(255, 255, 255);
    delay(1000);
    fillRing(0, 255, 0);
    delay(1000);
    fillRing(255, 0, 0);
    delay(1000);
}