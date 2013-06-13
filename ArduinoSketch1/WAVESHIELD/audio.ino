/*

Outputs on Mega to WaveShield: 28, 29, 30
Inputs on Arduino with WaveShield: 6, 7, 8

*/

#include <WaveHC.h>
#include <WaveUtil.h>

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader f;      // This holds the information for the file we're play

WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

uint8_t dirLevel; // indent level for file/dir names    (for prettyprinting)
dir_t dirBuf;     // buffer for directory reads

/*
 * Define macro to put error messages in flash memory
 */
#define error(msg) error_P(PSTR(msg))

// Function definitions (we define them here, but the code is below)
void play(FatReader &dir);

//////////////////////////////////// SETUP
void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps for debugging
  
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  
  putstring_nl("\nWave test!");  // say we woke up!
  
  putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
  Serial.println(FreeRam());

  //  if (!card.init(true)) { //play with 4 MHz spi if 8MHz isn't working for you
  if (!card.init()) {         //play with 8 MHz spi (default faster!)  
    error("Card init. failed!");  // Something went wrong, lets print out why
  }
  
  // enable optimize read - some cards may timeout. Disable if you're having problems
  card.partialBlockRead(true);
  
  // Now we will look for a FAT partition!
  uint8_t part;
  for (part = 0; part < 5; part++) {   // we have up to 5 slots to look in
    if (vol.init(card, part)) 
      break;                           // we found one, lets bail
  }
  if (part == 5) {                     // if we ended up not finding one  :(
    error("No valid FAT partition!");  // Something went wrong, lets print out why
  }
  
  // Lets tell the user about what we found
  putstring("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(), DEC);     // FAT16 or FAT32?
  
  // Try to open the root directory
  if (!root.openRoot(vol)) {
    error("Can't open root dir!");      // Something went wrong,
  }
  
  // Whew! We got past the tough parts.
  putstring_nl("Files found (* = fragmented):");

  // Print out all of the files in all the directories.
  root.ls(LS_R | LS_FLAG_FRAGMENTED);
}

//////////////////////////////////// LOOP
void loop() {
    
   int pin6 = digitalRead(6);
   int pin7 = digitalRead(7);
   int pin8 = digitalRead(8);
   

   //Welcome tone

   if (pin6 > 0) {
      int hi = random(0, 3);
         switch(hi){
           case 0:
            playfile("WELCOME.WAV");
            Serial.println("got here 0 1");
                while (wave.isplaying) {} // do nothing while WAV is playing
                wave.stop();  // stop when its over
                Serial.println("got here");
           break;
           
           case 1:
            playfile("WELCOMEB.WAV");
            Serial.println("got here 0 2");
                while (wave.isplaying) {} // do nothing while WAV is playing
                wave.stop();  // stop when its over
           break;
           
           case 2:
            playfile("TWOB.WAV");
            Serial.println("got here 0 3");
                while (wave.isplaying) {} // do nothing while WAV is playing
                wave.stop();  // stop when its over
           break;
           
           default:
           break;
         }
   }
  

  //Middle Tone
  if (pin7 > 0) {
    
   int mid = random (0, 8);
    switch(mid){
           case 0:
            playfile("TWO.WAV");
            Serial.println("got here 1 1");
                while (wave.isplaying) {} // do nothing while WAV is playing
                wave.stop();  // stop when its over
           break;
           
           case 1:
            playfile("THREE.WAV");
            Serial.println("got here");
                while (wave.isplaying) {} // do nothing while WAV is playing
                wave.stop();  // stop when its over
           break;
           
           case 2:
            playfile("THREEB.WAV");
            Serial.println("got here 1 2");
                while (wave.isplaying) {} // do nothing while WAV is playing
                wave.stop();  // stop when its over
           break;
           
           case 3:
            playfile("FIVE.WAV");
            Serial.println("got here 1 3");
                while (wave.isplaying) {} // do nothing while WAV is playing
                wave.stop();  // stop when its over
           break;
           
           case 4:
            playfile("ONE.WAV");
            Serial.println("got here 1 4");
                while (wave.isplaying) {} // do nothing while WAV is playing
                wave.stop();  // stop when its over
           break;
           
           case 5:
            playfile("SIX.WAV");
            Serial.println("got here 1 5");
                while (wave.isplaying) {} // do nothing while WAV is playing
                wave.stop();  // stop when its over
           break;
           
           case 6:
            playfile("NINE.WAV");
            Serial.println("got here 1 6");
                while (wave.isplaying) {} // do nothing while WAV is playing
                wave.stop();  // stop when its over
           break;
           
           case 7:
            playfile("TEN.WAV");
            Serial.println("got here 1 7");
                while (wave.isplaying) {} // do nothing while WAV is playing
                wave.stop();  // stop when its over
           break;
           
           default:
           break;
         }
  }
  
  
  //Leaving tone
  if (pin8 > 0) {
    int left = random (0, 2);
     switch(left){
           case 0:
            playfile("I.WAV");
            Serial.println("got here 2 1");
                while (wave.isplaying) {} // do nothing while WAV is playing
                wave.stop();  // stop when its over
           break;
           
           case 1:
            playfile("I2.WAV");
            Serial.println("got here 2 2");
                while (wave.isplaying) {} // do nothing while WAV is playing
                wave.stop();  // stop when its over
           break;
           
           default:
           break;
  }
}
}






/////////////////////////////////// HELPERS
/*
 * print error message and halt
 */
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}
/*
 * print error message and halt if SD I/O error, great for debugging!
 */
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}
/*
 * play recursively - possible stack overflow if subdirectories too nested
 */
void play(FatReader &dir) {
  FatReader file;
  while (dir.readDir(dirBuf) > 0) {    // Read every file in the directory one at a time
  
    // Skip it if not a subdirectory and not a .WAV file
    if (!DIR_IS_SUBDIR(dirBuf)
         && strncmp_P((char *)&dirBuf.name[8], PSTR("WAV"), 3)) {
      continue;
    }

    Serial.println();            // clear out a new line
    
    for (uint8_t i = 0; i < dirLevel; i++) {
       Serial.write(' ');       // this is for prettyprinting, put spaces in front
    }
    if (!file.open(vol, dirBuf)) {        // open the file in the directory
      error("file.open failed");          // something went wrong
    }
    
    if (file.isDir()) {                   // check if we opened a new directory
      putstring("Subdir: ");
      printEntryName(dirBuf);
      Serial.println();
      dirLevel += 2;                      // add more spaces
      // play files in subdirectory
      play(file);                         // recursive!
      dirLevel -= 2;    
    }
    else {
      // Aha! we found a file that isnt a directory
      putstring("Playing ");
      printEntryName(dirBuf);              // print it out
      if (!wave.create(file)) {            // Figure out, is it a WAV proper?
        putstring(" Not a valid WAV");     // ok skip it
      } else {
        Serial.println();                  // Hooray it IS a WAV proper!
        wave.play();                       // make some noise!
        
        uint8_t n = 0;
        while (wave.isplaying) {// playing occurs in interrupts, so we print dots in realtime
          putstring(".");
          if (!(++n % 32))Serial.println();
          delay(100);
        }       
        sdErrorCheck();                    // everything OK?
        // if (wave.errors)Serial.println(wave.errors);     // wave decoding errors
      }
    }
  }
}

// Plays a full file from beginning to end with no pause.
void playcomplete(char *name) {
  // call our helper to find and play this name
  playfile(name);
  while (wave.isplaying) {
  // do nothing while its playing
  }
  // now its done playing
}

void playfile(char *name) {
  // see if the wave object is currently doing something
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  // look in the root directory and open the file
  if (!f.open(root, name)) {
    putstring("Couldn't open file "); Serial.print(name); return;
  }
  // OK read the file and turn it into a wave object
  if (!wave.create(f)) {
    putstring_nl("Not a valid WAV"); return;
  }
  
  // ok time to play! start playback
  wave.play();
}
