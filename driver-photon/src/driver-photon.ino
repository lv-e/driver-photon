#include "display/lcd.h"

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