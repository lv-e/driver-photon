#include "lv-driver/driver.h"
#include "lv-engine/engine.h"
#include "lv-game/bootstrap.h"

SYSTEM_MODE(MANUAL);
bool onlineTrigger = false;

void setup() {

  // setup SPI & timers
  LCD::shared().setup();
  lvDisplay.clear(0);

  pinMode(D6, INPUT_PULLDOWN);
  pinMode(D7, OUTPUT);
  
  setupScenes();
  lvDirector.runScene(SCENE_MAIN);
  
}

void loop() {

  if (!onlineTrigger) {  

    // wait for v-sync
    if (!LCD::shared().waitingFrame()) return;

    SINGLE_THREADED_BLOCK(){

      // then draw frame buffer to display
      LCD::shared().beginDrawing();
      lvDirector.draw();
      LCD::shared().endDrawing();
    }

    // and update game logic
    // leaving graphics ready for next v-sync
    lvDirector.update();
  } 

  if (lvGamepads.isUp(lvGamepad(0).select)) {
    lvDirector.runScene(NO_SCENE);
    onlineTrigger = true;
    pinSetFast(D7);
    Particle.connect();
  }

  if (onlineTrigger && Particle.connected()) Particle.process();
  
}