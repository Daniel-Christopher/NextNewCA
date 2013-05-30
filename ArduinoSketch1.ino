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

//data structure for storing rgb values
struct RGB {
  byte r;
  byte g;
  byte b;
};

//RGB data structure reused for pin assignments in stripSet
//because making a seperate struct would be redundant
struct stripSet {
  RGB one;
  RGB two;
  RGB three;
  RGB four;
};
//example stripSet access: ledStrips.one.r = 255;

// Number of RGB LEDs in DALED strand:
const nLEDs = 154;

//2 pins for DALED output
const byte DATAPIN = 2;
const byte CLOCKPIN = 3;

// DALED Initializer: first param is the number of LEDs in the strand. Next two are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

//LED pins for each strip
const RGB stripOne =  {0,1,2};
const RGB stripTwo =  {3,4,5};
const RGB stripThree =  {6,7,8};
const RGB stripFour = {9,10,11};
//remember these const values are unchangeable by the program,
//and are used only to store pin assignments.

//stripSet of all LED strips
//again const
const stripSet ledStrips {stripOne, stripTwo, stripThree, stripFour};

//writes all colors to their corresponding strips
//example usage: writeStrips(stripSet, red, purple, orange, yellow);
void writeStrips( stripSet strips, RGB first, RGB second, RGB third, RGB fourth){
  writeStrip(strips.one, first);
  writeStrip(strips.two, second);
  writeStrip(strips.three, third);
  writeStrip(strips.four, fourth);
}
//helper function for writeStrips
void writeStrip(RGB strip, RGB color){
  analogWrite(strip.r, color.r);
  analogWrite(strip.g, color.g);
  analogWrite(strip.b, color.b);
}

//Number of times Final PIR activated
int i;

//PIR Trackers
int PIR[4];

//Counters
int PIRcount[4];


//Timing, used by readPIR()
uint startTime, startTimeforOveride, timeSinceLastOverride;
uint lastSense[4] ={0, 0, 0, 0};
bool coolingDown[4] = {false, false, false, false};
bool everOveridden;


/**********************/



void setup(){
  Serial.begin(9600);

  //Warm up for PIRs
  delay(7000);

  //kinda hacky due to using constants, but way more concise way of initializing
  for (int i = 0; i<12; i++){
    pinMode(i, OUTPUT);
  }

  // Start up the DALED strip
  strip.begin();

  // Update the DALED strip, to start they are all 'off'
  strip.show();
}



/**********************/

uint seconds(){
  return (uint) millis()/1000;
}

void loop(){
  //Reset program every day
  if (millis() > 86400){
  //Reset program when uint from seconds overflows
  if (seconds() == 0){
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
  
  startTime = seconds();

  for (j=0; j<4; j++){
    if (PIR[j] == HIGH && !(coolingDown[j])){
      PIRcount[j]++;
      lastSense[j] = seconds();
      coolingDown[j] = true;

        //if 4th PIR is HIGH, execute overideLED function
        if(PIR[j] == 3){
            i++; //different than PIRcount[3], as i never gets decremented, this is roughly how many people exited per day
            overideLEDs();
        }
    }else if (PIR[j] == HIGH && coolingDown[j]){
      //if PIR is HIGH and has been at least 30 seconds since last trigger, then increment counter
      if (startTime - lastSense[i] > 30){
         PIRcount[j]++;
         lastSense[j] = seconds();
         coolingDown[j] = true;
      }else{
        //Not Enough time has Elapsed
      }

    }else if (PIR[j] == LOW){
       //if PIR is HIGH and has been at least 60 seconds since last trigger, then decrement counter
      if (startTime - lastSense[j] > 60){
        if (PIRcount[j] > 0){
          PIRcount[j]--;
          lastSense[j] = seconds();
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
  startTimeforOveride = seconds();

  //Execute first time
  if (!everOveridden){
      // Blinky code here
      everOveridden = true;
      timeSinceLastOverride = seconds();

  //Executes only if more than two minutes has elapsed since last call
  }else if (everOveridden && (startTimeforOveride - timeSinceLastOverride) > 120){
      // Blinky code here
      timeSinceLastOverride = seconds();
  }else{
      return;
  }
}


int seconds(){
    int sec = millis()/1000;
    return sec;
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