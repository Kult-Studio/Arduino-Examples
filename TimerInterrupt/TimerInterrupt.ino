/* ==========================================================================
 * This program leaves and LED on for a set delay duration when signal 
 * is triggered from the input pin
 * Tested on Arduino Uno
 * ==========================================================================
 */
 
// Output LED used in this example is a neopixel and an i2c VL53L0X sensor
#include "Adafruit_VL53L0X.h"
#include <Adafruit_NeoPixel.h>

#define OUT_LED_PIN 5

#define NUMPIXELS 6
#define DELAY_DURATION  1000

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure;

Adafruit_NeoPixel pixels(NUMPIXELS, OUT_LED_PIN, NEO_GRB + NEO_KHZ800);

volatile bool ledState = false;
volatile unsigned short timeLeft = 0;

// --------------------------------------------------------------------------
// setup
// --------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (! Serial) { delay(1); }
    
  pixels.begin();
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
  
  Serial.println(F("Timer interrupt example\n\n")); 
}

// --------------------------------------------------------------------------
// loop
// --------------------------------------------------------------------------
void loop() {
  VL53L0X_RangingMeasurementData_t measure;
    
  Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
    if (measure.RangeMilliMeter < 50){
      timeLeft = DELAY_DURATION;
    }
  } else {
    Serial.println(" out of range ");
  }
  delay(100);
}

// --------------------------------------------------------------------------
// interrupt routine - called once a millisecond
// --------------------------------------------------------------------------
SIGNAL(TIMER0_COMPA_vect) {
  if (timeLeft > 0){
    timeLeft--;
    if (ledState != true){
      for(int i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 150, 0));
      }
      pixels.show();
      ledState = true;
    }
  }
  else if (ledState != false){
    pixels.clear();
    pixels.show();
    ledState = false;
  }
}
