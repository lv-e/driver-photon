
#include "lv-game/scene_main.h"
#include "lv-game/shared.h"
#include "lv-engine/engine.h"

int frame;

void scene_main_setup() {
    scenesTable[SCENE_MAIN] = {
        &scene_main_on_awake,
        &scene_main_on_enter,
        &scene_main_on_frame,
        &scene_main_on_exit
    };
}

void scene_main_on_awake() {

}

void scene_main_on_enter() {

}

void scene_main_on_frame() {
    lvDisplay.fillRect(6, 10, 10, 130 - 20, 130 - 20);
    lvDisplay.fillRect(11, 20, 20, 130 - 40, 130 - 40);
    lvDisplay.fillRect(19, 30, 30, 130 - 60, 130 - 60);
}

void scene_main_on_exit() {

}
