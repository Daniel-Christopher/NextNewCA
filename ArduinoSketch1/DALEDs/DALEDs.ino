/*

Outputs on Mega to WaveShield: 22, 23, 24
Inputs on Mini with WaveShield: 6, 7, 8

*/

#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number 

Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, 3, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  int pin6 = digitalRead(6);
  int pin7 = digitalRead(7);
  int pin8 = digitalRead(8);
  
  if (pin6 ==HIGH && pin7 == LOW && pin8==LOW){
        colorWipe(strip.Color(random(100, 155), random(0,26), random(0,100)), 100);
  }else if (pin7 ==HIGH && pin6 == LOW && pin8==LOW){
    
         colorWipe(strip.Color(random(40,155), random(0,155), random(0,155)), random(15, 60));
 
  }else if (pin8 ==HIGH && pin7 == LOW && pin6==LOW){
        
     colorWipe(strip.Color(random(40,155), random(0,155), random(0,155)), 75);
  
  }else {
      for(int a=0 ; a<strip.numPixels(); a++) strip.setPixelColor(a, 0);
  }
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
   int a, b, d;

  for(a=0 ; a<strip.numPixels(); a++) strip.setPixelColor(a, 0);

  for(a=76, b=77; a>0; a--, b++) {
      strip.setPixelColor(a, c); // Set new pixel 'on'
      strip.setPixelColor(b, c);
        if (b == 149){
            strip.setPixelColor(150, c);
        }    
      strip.show();
      delay(wait);
  }

  delay(3000);

  c = (random(80,155), random(50,100), random(20,155));

  for(a=0, b=150; a<77; a++, b--) {
      strip.setPixelColor(a, c); // Set new pixel 'on'
      strip.setPixelColor(b, c);
      strip.show();
      delay(wait);
  }

  delay(3000);

  for(a=76, b=77; a>0; a--, b++) {
    strip.setPixelColor(76, 0); 
      strip.setPixelColor(a, 0);
      strip.setPixelColor(b, 0);
      strip.show();
      delay(wait);
  }

}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait * 2);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 155 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 155 - WheelPos * 3);
  }
}
