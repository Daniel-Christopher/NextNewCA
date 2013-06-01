//data structure for storing rgb values
struct RGB {
  byte r;
  byte g;
  byte b;
};

//RGB data structure reused for pin assignments in stripSet
//example stripSet access: ledStrips.one.r = 255;
struct stripSet {
  RGB one;
  RGB two;
  RGB three;
  RGB four;
};
//Not sure where these should go but we'll need them to define colors and for pulsing effect
//Colors...
const RGB WHITE = {255, 255, 255};//white
const RGB RED = {255, 0, 0};//red
const RGB YELLOW = {255, 50, 0};//yellow
const RGB GREEN = {0, 255, 0};//green
const RGB CYAN = {0, 230, 255};//cyan
const RGB BLUE = {0, 0, 255};//blue
const RGB VIOLET = {255, 20, 147};//violet
const RGB OFF = {0,0,0}; //off
