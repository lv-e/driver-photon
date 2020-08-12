/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/lv/Github/driver-photon/driver-photon/src/driver-photon.ino"
#include "display/lcd.h"

void setup();
void loop();
#line 3 "/Users/lv/Github/driver-photon/driver-photon/src/driver-photon.ino"
LCD lcd;
unsigned int framecounter = 0;

void setup() {
  pinMode(D7, OUTPUT);
  lcd.setup();
  lcd.clear(0b1110000000000111);
}

void loop() {
  
  switch ((framecounter++) % 100){
    case 0: pinSetFast(D7); break;
    case 50: pinResetFast(D7); break;
    default: break;
  }

  lcd.loop();
}