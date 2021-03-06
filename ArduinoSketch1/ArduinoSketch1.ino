/******** i++ **********//*


6/2013
Authors: Daniel Christopher (daniel.adam.christopher@gmail.com) & Aidan Seine (aidanseine@gmail.com)

This sketch modifies a ColorChasing function from Adafruit's LPD8806 example code
uses parts of Color Library for Project Socialites written by Ace Levenberg (acelevenberg@gmail.com) & Aidan Seine

/*** INSTALLATION SPECS ****//*

HARDWARE:
- 4 PIRS connected in parallel, being read by digital pins 50-53 (see function readPIRs)
- 4 RGB LED strips in parallel, [2-4] is Strip1, [5-7] is Strip2, [8-10] is Strip3, [11-13] is Strip4
- 1 continuous strip of digital addressable RGB LEDs (DALED), 154 in total, installed above exit door

*//**********************/


#include "SPI.h"
#include "RGBstrips.h"


//LED pins for each strip
const RGB stripOne = {2,3,4};
const RGB stripTwo = {5,6,7};
const RGB stripThree = {8,9,10};
const RGB stripFour = {11,12,13};

//stripSet of all LED strips
const stripSet ledStrips = {stripOne, stripTwo, stripThree, stripFour};

//Number of times Final PIR activated
int i;

//PIR Trackers
int PIR[4];

//Counters
int PIRcount[4];

//Timing, used by readPIR()
uint16_t startTime, startTimeforOveride, timeSinceLastOverride;
uint16_t lastSense[4] ={0, 0, 0, 0};

//whether or not PIR is in 30second cool down period
bool coolingDown[4] = {false, false, false, false};
//keeps track of whether value of PIRcount is rising (true) or falling (false)
bool rising[4] = {false, false, false, false};
bool everOveridden;
RGB prevColor = {0,0,0};

/**********************/

void setup(){
  Serial.begin(9600);

  //Warm up for PIRs
  delay(7000);

  //concise way of initializing
  for (int j = 3; j<14; j++){
    pinMode(j, OUTPUT);
  }

  for (int j = 25; j<28; j++){
  pinMode(j, OUTPUT);
  }
}


/**********************/

void loop(){

  readPIRS();
  
  prevColor = stripDance(ledStrips, PIRcount, prevColor);
  //reverseCascade(ledStrips, RED, BLUE, 10);
  //writeStrips(ledStrips,RED, BLUE, YELLOW, VIOLET);
  // Among the things I tried to play sound
  if(PIRcount[0] > 0){
    digitalWrite(28, HIGH);
    digitalWrite(23, HIGH);
    //delay(5000);
    digitalWrite(28, LOW);
    digitalWrite(23, LOW);
    }
  if(PIRcount[1] > 0){
    digitalWrite(29, HIGH);
    digitalWrite(23, HIGH);
    //delay(5000);
    digitalWrite(29, LOW);
    digitalWrite(23, LOW);
  }
        
  if(PIRcount[3] > 0){
    digitalWrite(30, HIGH);
    digitalWrite(23, HIGH);
    //delay(5000);
    digitalWrite(30, LOW);
    digitalWrite(23, LOW);
  }

  //Reset program when uint from seconds overflows
  if (seconds() == 0){
     /* Code Snippet from "user HULK" */
    void(* resetFunc) (void) = 0; //declare reset function @ address 0
    resetFunc();
  }
}


/**********************/

//convert millis to seconds
uint16_t seconds(){
  return millis()/1000;
}


//Read PIRS and keep track of which PIRs are active & generally how long they have been active
//ex. a PIRcount == 1 is appox 30 seconds
int readPIRS(){
  int j;
  PIR[0] = digitalRead(50);
  PIR[1] = digitalRead(51);
  PIR[2] = digitalRead(52);
  PIR[3] = digitalRead(53);
  
  startTime = seconds();

  if (PIR[0] == LOW && PIR[1] == LOW && PIR[2] == LOW && PIR[3] == LOW){
    return 1;
  }

  for (j=0; j<4; j++){
    if (PIR[j] == HIGH && !(coolingDown[j])){
      PIRcount[j]++;
      lastSense[j] = seconds();
      coolingDown[j] = true;



        //if 4th PIR is HIGH, execute overideLED function
        if(PIR[3] == HIGH){
            i++; //different than PIRcount[3], as i never gets decremented, this is roughly how many people exited per day
           
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


void writeStrips(stripSet strips, RGB first, RGB second, RGB third, RGB fourth){
  writeStrip(strips.one, first);
  writeStrip(strips.two, second);
  writeStrip(strips.three, third);
  writeStrip(strips.four, fourth);
}


//helper function
void writeStrip(RGB strip, RGB color){
  analogWrite(strip.r, color.r);
  analogWrite(strip.g, color.g);
  analogWrite(strip.b, color.b);
}

void colorFade(RGB strip, RGB colorOne, RGB colorTwo, int rate){
 RGB color = colorOne;
 while(!rgbEquals(color, colorTwo)){
   Serial.println("struck in this loop");
   writeStrip(strip , color);
   delay(rate);
   color = chamillionaire(color, colorTwo);
 } 
 Serial.println("broke out of loop");
  
}

RGB chamillionaire(RGB oldColor, RGB newColor){
  oldColor.r = incDec(oldColor.r, newColor.r);
  oldColor.g = incDec(oldColor.g, newColor.g);
  oldColor.b = incDec(oldColor.b, newColor.b);
  return oldColor;
}

//helper function to increment or decrement by 1.
int incDec(int oldVal, int newVal){
  if (oldVal > newVal){
    return constrain(oldVal-3, newVal, oldVal);
    //return oldVal-1;
  }
  if (oldVal < newVal){
    return constrain(oldVal+3, oldVal, newVal);
    //return oldVal+1;
  }
  return oldVal;
}

//checks if two RGB's are the same
bool rgbEquals(RGB colorOne, RGB colorTwo){
  if(
  colorOne.r == colorTwo.r &&
  colorOne.g == colorTwo.g &&
  colorOne.b == colorTwo.b) return true;
  return false;
}
void pulseStrip(RGB strip, RGB color, int rate){
  for (int fadeValue = 0; fadeValue <= 245; fadeValue += 5){ 
    RGB hue = {
    map(color.r-fadeValue, color.r-255, color.r, 0, color.r),
    map(color.g-fadeValue, color.g-255, color.g, 0, color.g),
    map(color.b-fadeValue, color.b-255, color.b, 0, color.b)};
    writeStrip(strip, hue);
    delay(rate);  
  }
    for (int fadeValue = 245; fadeValue >= 0 ; fadeValue -= 5){
    RGB hue = {
    map(color.r-fadeValue, color.r-255, color.r, 0, color.r),
    map(color.g-fadeValue, color.g-255, color.g, 0, color.g),
    map(color.b-fadeValue, color.b-255, color.b, 0, color.b)};
    writeStrip(strip, hue);
    delay(rate);  
  }
}

void colorCascade(stripSet strips, RGB colorOne, RGB colorTwo, int rate){
  colorFade(strips.one, colorOne, colorTwo, rate);
  colorFade(strips.two, colorOne, colorTwo, rate);
  colorFade(strips.three, colorOne, colorTwo, rate);
  colorFade(strips.four, colorOne, colorTwo, rate);
}

void reverseCascade(stripSet strips, RGB colorOne, RGB colorTwo, int rate){
  colorFade(strips.four, colorOne, colorTwo, rate);
  colorFade(strips.three, colorOne, colorTwo, rate);
  colorFade(strips.two, colorOne, colorTwo, rate);
  colorFade(strips.one, colorOne, colorTwo, rate);
}

RGB stripDance(stripSet strips, int counts[4], RGB oldColor){
  int count = counts[0]+counts[1]+counts[2]+counts[3];
  for (int j = 0; j<4; j++){
    int rate = calcRate(count);
    RGB color = calcRgb(counts);
    reverseCascade(strips, oldColor, color, rate);
    oldColor = color;
  }
  return oldColor;
}

int calcRate (int count){
  return map(count, 0, 10, 10, 0);
}
RGB calcRgb(int counts[4]){
  RGB color;
  color.r = map(counts[0], 0, random(1, 10), 0, 255);
  color.g = map(counts[1], 0, random(1, 10), 0, 255);
  color.b = map(counts[2], 0, random(1, 10), 0, 255);
  return color; 
}

void letsGoCrazy(stripSet strips){
  for(int j = 0; j<10; j++){
    RGB colorOne = {random(255), random(255), random(255)};
    RGB colorTwo = {random(255), random(255), random(255)};
    colorCascade(strips, colorOne, colorTwo, j);
    reverseCascade(strips, colorOne, colorTwo, j);
  }
}


//Do something crazy that affects all LEDs when person walks through exit door
// Approx 5-10 second light effect
void overideLEDs(){

  //Only allow one time per 2 minutes
  startTimeforOveride = seconds();

  //Execute first time
  if (!everOveridden){

      letsGoCrazy(ledStrips);
      everOveridden = true;
      timeSinceLastOverride = seconds();
       digitalWrite(30, HIGH);
        digitalWrite(24, HIGH);
       delay(5000);
       digitalWrite(30, LOW);
         digitalWrite(24, LOW);

  //Executes only if more than two minutes has elapsed since last call
  }else if (everOveridden && (startTimeforOveride - timeSinceLastOverride) > 120){
      letsGoCrazy(ledStrips);
      digitalWrite(30, HIGH);
       digitalWrite(24, HIGH);
      delay(5000);
      digitalWrite(30, LOW);
       digitalWrite(24, LOW);
      timeSinceLastOverride = seconds();
  }else{
      return;
  }
}

