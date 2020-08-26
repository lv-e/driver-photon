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

        if (x > 124 || x < 4) sx *= -1;
        if (y > 124 || y < 4) sy *= -1;

        lvDisplay.fillRect(color, x, y, 2, 2);
    }
};

Ball balls[1024];

void MainScene_onAwake(void){
}

void MainScene_onEnter(void){
    for (int i = 0; i < 1024; i++) {
        float rand = ((float)((*(int *)0xE0001004) % 1024 + 1))/1024.0 - 0.5;
        balls[i].set( 130/2, 130/2,
                      rand + sin((float)i/(float)5.0) * 2, rand + cos((float)i/(float)5.0) * 2,
                      1 + i%31 );
    }
}

void MainScene_onFrame(void){
    lvDisplay.fillRect(0, 0, 0, 130, 130);
    for(short i = 0; i < 1024 ; i++) balls[i].step();
}

void MainScene_onExit(void) {

}

void MainScene_setup(){
    scenesTable[MainSceneID] = {
        &MainScene_onAwake,
        &MainScene_onEnter,
        &MainScene_onFrame,
        &MainScene_onExit
    };
}