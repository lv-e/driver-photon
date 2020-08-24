#include "driver.h"
#include "engine.h"
#include "main_scene.h"

SYSTEM_MODE(MANUAL);
bool onlineTrigger = false;

void setup() {

  // setup SPI & timers
  LCD::shared().setup();
  lvDisplay.clear();

  pinMode(D6, INPUT_PULLDOWN);
  
  MainScene_setup();
  lvDirector.runScene(MainSceneID);
}

void loop() {

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

  if (digitalRead(D6) == HIGH) onlineTrigger = true;
  if (onlineTrigger) pinSetFast(D7);
  if (onlineTrigger) Particle.connect();
  if (Particle.connected()) Particle.process();

}