#include "lv-driver/driver.h"
#include "lv-engine/engine.h"
#include "lv-game/scene_main.h"

SYSTEM_MODE(MANUAL);
bool onlineTrigger = false;

unsigned long ptime;
int fps;

void setup() {

  // setup SPI & timers
  LCD::shared().setup();
  lvDisplay.clear();

  pinMode(D6, INPUT_PULLDOWN);
  
  scene_main_setup();
  lvDirector.runScene(SCENE_MAIN);

  ptime = millis();
  fps = 0;
  Particle.variable("fps", &fps, INT);
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

    unsigned long ctime = millis();
    unsigned long diff = ctime - ptime;
    fps = (int) (1. / (((float) diff)/ 1000.0));
    ptime = ctime;

  } 

  if (digitalRead(D6) == HIGH) onlineTrigger = true;
  if (onlineTrigger) pinSetFast(D7);
  if (onlineTrigger) Particle.connect();
  if (Particle.connected()) Particle.process();
  
}