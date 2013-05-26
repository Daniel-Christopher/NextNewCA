/******** i++ **********//*

5/26/13
Authors: Daniel Christopher (daniel.adam.christopher@gmail.com) & Aidan Seine (aidanseine@gmail.com)

This sketch modifies a ColorChasing function from Adafruit's LPD8806 example code, and 
uses parts of Color Library for Project Socialites written by Ace Levenberg & Aidan Seine


*//**********************/

/* Libraries for DALEDs */
#include "LPD8806.h"
#include "SPI.h"

// Number of RGB LEDs in DALED strand:
#define nLEDs = 60

//2 pins for DALED output
#define DATAPIN  = 2
#define CLOCKPIN = 3

// DALED Initializer: first param is the number of LEDs in the strand.  Next two are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

//LED channels
#define R1 0
#define G1 1
#define B1 2
#define R2 3
#define G2 4
#define B2 5
#define R3 6
#define G3 7
#define B3 8
#define R4 9
#define G4 10
#define B4 11

//PIR Trackers
int PIR[4];

//Counters
int PIRcount[4];

//Timing, used by readPIR()
long startTime;
long lastSense[4] ={0, 0, 0, 0}; 
bool coolingDown[4] = {false, false, false, false};

void setup(){
  //Warm up for PIRs
  delay(7000);

  pinMode(R1, OUTPUT);
  pinMode(G1, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(G2, OUTPUT);
  pinMode(B2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(G3, OUTPUT);
  pinMode(B3, OUTPUT);
  pinMode(G4, OUTPUT);
  pinMode(R4, OUTPUT);
  pinMode(B4, OUTPUT);

   // Start up the DALED strip
  strip.begin(); 

  // Update the DALED strip, to start they are all 'off'
  strip.show();
}


void loop(){
  //Reset program every day
  if (millis() > 86400000){
     /* Code Snippet from "user HULK" */
    void(* resetFunc) (void) = 0; //declare reset function @ address 0
    resetFunc();
  }

  //Read PIRS
  readPIRS();

  //Then Activte LEDs
  activateLEDs();


  //Params: RGB color & speed in millis
  colorChase(strip.Color(127,  0, 127), 25); // Violet
  colorChase(strip.Color(127, 127, 127), 25); // Violet

}




//Read PIRS and Keep track of 
void readPIRS(){
  PIR[0] = digitalRead(50);
  PIR[1] = digitalRead(51);
  PIR[2] = digitalRead(52);
  PIR[3] = digitalRead(53);
  
  startTime = millis();

  for (int i=0; i<4; i++){
    if (PIR[i] == HIGH && !(coolingDown[i])){
      PIRcount[i]++;
      lastSense[i] = millis();
      coolingDown[i] = true;

    }else if (PIR[i] == HIGH && coolingDown[i]){
      //if PIR is HIGH and has been at least 30 seconds since last trigger, then increment counter
      if (startTime - lastSense[i] > 30000){
         PIRcount[i]++;
         lastSense[i] = millis();
         coolingDown[i] = true;
      }else{
        //Not Enough time has Elapsed
      }

    }else if (PIR[i] == LOW){
       //if PIR is HIGH and has been at least 60 seconds since last trigger, then decrement counter
      if (startTime - lastSense[i] > 60000){
        if (PIRcount[i] > 0){
          PIRcount[i]--;
          lastSense[i] = millis();
        }
      }
    }
  }
}



void activateLEDs(STRIP, COLOR, FADE){
}




// DALED Function - Chase one dot back and forth 
void colorChase(uint32_t c, uint8_t wait) {
  int i;

  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c); // Set new pixel 'on'
    strip.show();              // Refresh LED states
    strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
    delay(wait);
  }

  strip.show(); // Refresh to turn off last pixel
  
  
  // Start by turning all pixels off:
  for(i=strip.numPixels(); i>=0; i--) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for(i=strip.numPixels(); i>=0; i--) {
    strip.setPixelColor(i, c); // Set new pixel 'on'
    strip.show();              // Refresh LED states
    strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
    delay(wait);
  }
  strip.show(); // Refresh to turn off last pixel 
}