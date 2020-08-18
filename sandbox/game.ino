#include "driver.h"
#include "engine.h"

void setup() {
  // setup SPI & timers
  LCD::shared().setup();
}

void loop() {

  // wait for v-sync
  if (!LCD::shared().waitingFrame()) return;
  
  SINGLE_THREADED_BLOCK(){

    // then draw frame buffer to display
    LCD::shared().beginDrawing();
    lvDirector.draw();
    LCD::shared().endDrawing();

    // and update game logic
    // leaving graphics ready for next v-sync
    lvDirector.update();
  }
}