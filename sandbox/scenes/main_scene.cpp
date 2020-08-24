#include "main_scene.h"
#include "engine.h"
#include <math.h>

struct Ball {

    float x, y;
    float sx, sy;
    lv::octet color;

    Ball(){ }

    void set(int _x, int _y, float _sx, float _sy, lv::octet _c){
        x = _x;
        y = _y;
        sx = _sx;
        sy = _sy;
        color = _c;
    }

    void step(){
        
        x += sx;
        y += sy;

        if (x > 126 || x < 2) sx *= -1;
        if (y > 126 || y < 2) sy *= -1;

        lvDisplay.fillRect(color, x, ((int) y-12 )%130, 2, 24);
    }
};

Ball balls[128];

void MainScene_onEnter(void){
    for (int i = 0; i < 128; i++) {
        balls[i].set( 2 + (i*4), 60 + (sin((float)i/(float)5.0) * 30),
                      0, 1.0 + ((float)i / (float)128.0),
                      i%32 );
    }
}

void MainScene_onFrame(unsigned short frame){
    lvDisplay.fillRect(0, 0, 0, 130, 130);
    for(short i = 0; i < 30 ; i++) balls[i].step();
}

void MainScene_onExit(void) {

}

void MainScene_setup(){
    scenesTable[MainSceneID] = {
        &MainScene_onEnter,
        &MainScene_onFrame,
        &MainScene_onExit
    };
}