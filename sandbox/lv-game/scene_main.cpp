
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
    frame++;
}

void scene_main_on_exit() {

}
