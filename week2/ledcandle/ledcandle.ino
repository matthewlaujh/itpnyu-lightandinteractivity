#include "FastLED.h"
#include "FastLED_RGBW.h"

#define NUM_LEDS 7
#define DATA_PIN 11
#define BRIGHTNESS 200  

CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];

float ledBrightness[NUM_LEDS];  
float windEffect[NUM_LEDS];  
float windDirection = 1;  
float windStrength = 0.5;  
float waveOffset = 0;  
float speedMultiplier = 0.02;  
unsigned long nextMoveTime = 0;  
unsigned long holdTime = 0;  

void setup() { 
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
    FastLED.setBrightness(BRIGHTNESS);  

    for (int i = 0; i < NUM_LEDS; i++) {
        ledBrightness[i] = random(60, 120);
        windEffect[i] = 0;
    }

    FastLED.show();
}

void loop() {
    candleShadowMovement();
}

void candleShadowMovement() {
    static float time = 0.0;
    static unsigned long lastWindChange = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - lastWindChange > random(5000, 10000)) {  
        lastWindChange = currentMillis;
        windDirection = random(0, 2) * 2 - 1;  
        windStrength = random(4, 12) * 0.1; 
    }

    windStrength *= 0.9;  
    if (windStrength < 0.02) windStrength = 0;  

    if (currentMillis > nextMoveTime) {
        speedMultiplier = random(20, 200) * 0.001;  
        holdTime = random(200, 1000);  
        nextMoveTime = currentMillis + random(3000, 8000);  
    }

    waveOffset += windDirection * windStrength * speedMultiplier; 

    for (int i = 0; i < NUM_LEDS; i++) {
        int ledPos = i - (NUM_LEDS / 2);  

        float movement = sin(time + ledPos * 0.8 + waveOffset) * 120; 
        float nextMovement = sin(time + (ledPos + 1) * 0.8 + waveOffset) * 120;
        float prevMovement = sin(time + (ledPos - 1) * 0.8 + waveOffset) * 120;

        float blendedBrightness = (movement * 0.6) + (nextMovement * 0.2) + (prevMovement * 0.2);

        int brightness = constrain(ledBrightness[i] + blendedBrightness, 0, 180); 

        if (brightness < 30) {
            brightness = 0;
        }

        
        if (i == 0) {
         
            leds[i] = CRGBW(255, 140, 30, 20);  
        } else {
            leds[i] = CRGBW(255, 120, 40, 0);  
        }

        leds[i].r = brightness;
        leds[i].g = brightness * 0.4;
        leds[i].b = 0;
        if (i == 0) {
            leds[i].w = 20;  
        } else {
            leds[i].w = 0;
        }
    }

    FastLED.show();
    time += speedMultiplier;  
    delay(holdTime);  
}