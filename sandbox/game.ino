#include "driver.h"
#include "engine.h"

int frame = 0;

void setup() {
  LCD::shared().setup();
}

void loop() {

  if (LCD::shared().waitingFrame() == false) return;

  SINGLE_THREADED_BLOCK(){
    LCD::shared().beginDrawing();
    lv::director.draw();
    LCD::shared().endDrawing();
    lv::director.update();
  }

  // if ((frame++)%100 > 50) LCD::shared().clear(0b0001111110000101);
  // else LCD::shared().clear(0b1110000000000011);

  // LCD::shared().loop();

  
  // wait for frame
  
}