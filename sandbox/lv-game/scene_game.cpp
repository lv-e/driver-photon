
#include "lv-game/scene_game.h"
#include "lv-engine/engine.h"

// declarations
int xframe;

void scene_game_setup() {
    scenesTable[SCENE_GAME] = {
        &scene_game_on_awake,
        &scene_game_on_enter,
        &scene_game_on_frame,
        &scene_game_on_exit
    };
}

void scene_game_on_awake() {
    // on awake
}

void scene_game_on_enter() {
    // on enter
    xframe = 0;
}

void scene_game_on_frame() {
    lvDisplay.clear((xframe/5)%28);
    xframe++;

    if(xframe > 90) lvDirector.runScene(SCENE_MAIN);
}

void scene_game_on_exit() {

}
