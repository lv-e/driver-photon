
#include "driver.h"

LCD lcd;

void setup() {
  pinMode(D7, OUTPUT);
  lcd.setup();
  lcd.clear(0b0001111110000101);
}

void loop() {
  lcd.loop();
}