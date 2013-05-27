/******** i++ **********//*

6/2013
Authors: Daniel Christopher (daniel.adam.christopher@gmail.com) & Aidan Seine (aidanseine@gmail.com)

This sketch modifies a ColorChasing function from Adafruit's LPD8806 example code, and
uses parts of Color Library for Project Socialites written by Ace Levenberg (acelevenberg@gmail.com) & Aidan Seine



/*** INSTALLATION SPECS ****//*

HARDWARE:
- 4 PIRS connected in parallel, being read by digital pins 50-53 (see function readPIRs)
- 4 RGB LED strips in parallel, [0-2] is Strip1, [3-5] is Strip2, [6-8] is Strip3, [9-11] is Strip4
- 1 continuous strip of digital addressable RGB LEDs (DALED), 154 in total, installed above exit door

*//**********************/


/* Libraries for DALEDs */
#include "LPD8806.h"
#include "SPI.h"

// Number of RGB LEDs in DALED strand:
#define nLEDs = 154

//2 pins for DALED output
#define DATAPIN = 2
#define CLOCKPIN = 3

// DALED Initializer: first param is the number of LEDs in the strand. Next two are SPI data and clock pins:
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

//Number of times Final PIR activated
int i;

//PIR Trackers
int PIR[4];

//Counters
int PIRcount[4];


//AIDAN Q: Will these longs take up too much memory? FYI: in main loop, Arduino resets program everyday

//Timing, used by readPIR()
long startTime, startTimeforOveride, timeSinceLastOverride;
long lastSense[4] ={0, 0, 0, 0};
bool coolingDown[4] = {false, false, false, false};
bool everOveridden;



/**********************/



void setup(){
  Serial.begin(9600);

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



/**********************/



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

  //Output activity to Pi
  writePi();
}



/**********************/



//Read PIRS and keep track of which PIRs are active & generally how long they have been active
//ex. a PIRcount == 1 is appox 30 seconds
void readPIRS(){
  int j;
  PIR[0] = digitalRead(50);
  PIR[1] = digitalRead(51);
  PIR[2] = digitalRead(52);
  PIR[3] = digitalRead(53);
  
  startTime = millis();

  for (j=0; j<4; j++){
    if (PIR[j] == HIGH && !(coolingDown[j])){
      PIRcount[j]++;
      lastSense[j] = millis();
      coolingDown[j] = true;

        //if 4th PIR is HIGH, execute overideLED function
        if(PIR[j] == 3){
            i++; //different than PIRcount[3], as i never gets decremented, this is roughly how many people exited per day
            overideLEDs();
        }
    }else if (PIR[j] == HIGH && coolingDown[j]){
      //if PIR is HIGH and has been at least 30 seconds since last trigger, then increment counter
      if (startTime - lastSense[i] > 30000){
         PIRcount[j]++;
         lastSense[j] = millis();
         coolingDown[j] = true;
      }else{
        //Not Enough time has Elapsed
      }

    }else if (PIR[j] == LOW){
       //if PIR is HIGH and has been at least 60 seconds since last trigger, then decrement counter
      if (startTime - lastSense[j] > 60000){
        if (PIRcount[j] > 0){
          PIRcount[j]--;
          lastSense[j] = millis();
        }
      }
    }
  }
}


//Still in Progress...
int processPIRs(){

  //None active
  if (PIRcount[0] == 0 && PIRcount[1] == 0 && PIRcount[2] == 0 && PIRcount[3] == 0){
      return 0;

  //First PIR active only
  }else if (PIRcount[0] > 0 && PIRcount[1] < 1 && PIRcount[2] < 1 && PIRcount[3] < 1){
      return 1;

  //Two or More PIRs active, (includes first PIR)
  }else if (PIRcount[0] > 0 && PIRcount[1] > 0 || PIRcount[2] > 0 || PIRcount[3] > 0){
      return 2;

  //First PIR Inactive, one other PIR active
  }else if (PIRcount[0] < 0 && PIRcount[1] > 0 || PIRcount[2] > 0 || PIRcount[3] > 0){
      return 3;
  }
}



//Do something crazy that affects all LEDs when person walks through exit door
// Approx 5-10 second light effect
void overideLEDs(){

  //Only allow one time per 2 minutes
  startTimeforOveride = millis();

  //Execute first time
  if (!everOveridden){
      // Blinky code here
      everOveridden = true;
      timeSinceLastOverride = millis();

  //Executes only if more than two minutes has elapsed since last call
  }else if (everOveridden && (startTimeforOveride - timeSinceLastOverride) > 120000){
      // Blinky code here
      timeSinceLastOverride = millis();

  }else{
      return;
  }
}



//NEED HELP WITH THIS FUNCTION AND ONE BELOW
void activateLEDs(){
  switch (processPIRs()){
        case '0': return;
        case '1':
              LEDhelper();

        case '2'

        case '3'
        default: break;
    }
  }
}


void LEDhelper(){





  //Runs DALED function above exit door frame. I'm thinking this will happen when case == 0 from function above
  //Params: RGB color & speed in millis
  colorChase(strip.Color(127, 0, 127), 25); // Violet, 25 milliseconds
}



//Still unclear what to send PI
void writePi(){
  Serial.println("PIR Activity: ");

  //Ideally we'll tell the Pi when to take a photo here
  Serial.print("EXIT DOOR: ");
    if (PIRcount[3] > 0){
      Serial.println("ACTIVE");
    }else{
      Serial.println("INACTIVE");
    }
}




// DALED Function - Chase two dots back and forth

/*********** 154 ***********/
/*    <-- 76     77 -->    */
/*                         */
/*                         */
/*                         */
/*          EXIT           */
/*        DOOR FRAME       */
/*                         */
/*                         */
/*                         */
/*                         */
/* 0                   153 */


void colorChase(uint32_t c, uint8_t wait) {
  int a, b;

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
  for(a=0, b=153; a>78; a++, b--) {
    strip.setPixelColor(a, c); // Set new pixel 'on'
    strip.setPixelColor(b, c);
      strip.show(); // Refresh LED states
    strip.setPixelColor(a, 0); // Erase pixel, but don't refresh
    strip.setPixelColor(b, 0);
     delay(wait);
  }
  strip.show(); // Refresh to turn off last pixel
}