#include "lv-driver/driver.h"
#include "lv-engine/engine.h"
#include "lv-game/scene_main.h"

SYSTEM_MODE(MANUAL);
bool onlineTrigger = false;

void setup() {

  // setup SPI & timers
  LCD::shared().setup();
  lvDisplay.clear(0);

  pinMode(D6, INPUT_PULLDOWN);
  pinMode(D7, OUTPUT);
  
  scene_main_setup();
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

  if (digitalRead(D6) == HIGH || lvGamepads.isUp(lvGamepad(0).select)) onlineTrigger = true;
  if (onlineTrigger) pinSetFast(D7);
  if (onlineTrigger) Particle.connect();
  if (Particle.connected()) Particle.process();
  
}