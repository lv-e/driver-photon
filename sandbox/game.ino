
#include "driver.h"

LCD lcd;
int frame = 0;

void setup() {
  pinMode(D7, OUTPUT);
  lcd.setup();
}

void loop() {
  if ((frame++)%100 > 50) lcd.clear(0b0001111110000101);
  else lcd.clear(0b1110000000000011);

  lcd.loop();
}