/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/private/tmp/spice/src/main.ino"

void setup();
void loop();
#line 2 "/private/tmp/spice/src/main.ino"
void setup() {
  pinMode(D7, OUTPUT);
}

void loop() {
  delay(200);
  pinSetFast(D7);
  delay(200);
  pinResetFast(D7);
  delay(100);
  pinSetFast(D7);
  delay(100);
  pinResetFast(D7);
}