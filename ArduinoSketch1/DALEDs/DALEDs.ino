/* Libraries for DALEDs */
#include "LPD8806.h"
#include "SPI.h"

// Number of RGB LEDs in DALED strand:
const int nLEDs = 154;

//2 pins for DALED output
const byte DATAPIN = 2;
const byte CLOCKPIN = 3;

// DALED Initializer: first param is the number of LEDs in the strand. Next two are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, DATAPIN, CLOCKPIN);

int r, g, b, rate;

void setup(){
  for (int j = 7; j < 10; j++){
    pinMode(j, OUTPUT);
  }
 
   // Start up the DALED strip
  strip.begin();
  // Update the DALED strip, to start they are all 'off'
  strip.show();
  
}


void loop(){
    r = random (254); g = random(254); b = random(254); rate = random(30, 46);

    
    if (digitalRead(7)){
    daledAnimation(strip.Color(r, g, b), 15); // Violet, 25 milliseconds
    }else if (digitalRead(8)){
    daledAnimation(strip.Color(r, g, b), rate); // Violet, 25 milliseconds 
    }else{
    }
}


void daledAnimation(uint32_t c, uint8_t wait) {
  int a, b;

  //1st - color chase animation
  // Start by turning all pixels off:
  for(a=0 ; a<strip.numPixels(); a++) strip.setPixelColor(a, 0);

  // Then display two pixels at a time:
  for(a=77, b=78; a>0; a--, b++) {
    strip.setPixelColor(a, c); // Set new pixel 'on'
    strip.setPixelColor(b, c);
     strip.show(); // Refresh LED states
    strip.setPixelColor(a, 0); // Erase pixel, but don't refresh
    strip.setPixelColor(b, 0);
     delay(wait);
  }

  strip.show(); // Refresh to turn off last pixel
  
  // Start by turning all pixels off:
  for(a=0 ; a<strip.numPixels(); a++) strip.setPixelColor(a, 0);


   // Then display two pixels at a time:
  for(a=0, b=153; a<78; a++, b--) {
    strip.setPixelColor(a, c); // Set new pixel 'on'
    strip.setPixelColor(b, c);
      strip.show(); // Refresh LED states
    strip.setPixelColor(a, 0); // Erase pixel, but don't refresh
    strip.setPixelColor(b, 0);
     delay(wait);
  }
  strip.show(); // Refresh to turn off last pixel

  //2nd - color chase animation
  //colorWipe animation
  for (a=77, b=78; a>0; a--, b++) {
      strip.setPixelColor(a, c);
      strip.setPixelColor(b, c);
      strip.show();
      delay(wait*2);
  }

  for(a=0, b=153; a<78; a++, b--) {
      strip.setPixelColor(a, c);
      strip.setPixelColor(b, c);
      strip.show();
      delay(wait*2);
  }


  //turn off
  for(a=0 ; a<strip.numPixels(); a++) strip.setPixelColor(a, 0);
  strip.show(); // Refresh to turn off last pixel
}

