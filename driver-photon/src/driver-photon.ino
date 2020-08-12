#include "display/lcd.h"

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