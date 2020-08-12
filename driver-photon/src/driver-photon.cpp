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

void setup() {
  pinMode(D7, OUTPUT);
  lcd.setup();
}

void loop() {

  pinSetFast(D7);
  delay(100);
  pinResetFast(D7);
  delay(500);

  lcd.clear(0b0000101111000000);
  lcd.loop();
}