#include "game-pad-probe.h"
#include "lv-engine/engine.h"

#define DPAD_LATCH D4
#define DPAD_CLOCK D5
#define DPAD_DATA D2 
#define DPAD_WAIT_TIME_US 6

typedef struct GamePadProbe {
    unsigned short now;
    GamePadProbe();
    void query(void);
} GamePadProbe;

static GamePadProbe probe;

lv::GamePad lvDriver_GamePadState(lv::half player){
    
    lv::GamePad p = {0};
    if (player != 0 ) return p;

    probe.query();
    
    p.up    = probe.now & (1 << 4) ? 1 : 0;
    p.down  = probe.now & (1 << 5) ? 1 : 0;
    p.left  = probe.now & (1 << 6) ? 1 : 0;
    p.rigth = probe.now & (1 << 7) ? 1 : 0;

    p.x     = probe.now & (1 << 9) ? 1 : 0;
    p.y     = probe.now & (1 << 1) ? 1 : 0;
    p.a     = probe.now & (1 << 8) ? 1 : 0;
    p.b     = probe.now & (1 << 0) ? 1 : 0;

    p.l = probe.now & (1 << 10) ? 1 : 0;
    p.r = probe.now & (1 << 11) ? 1 : 0;

    p.start     = probe.now & (1 << 3) ? 1 : 0;
    p.select    = probe.now & (1 << 2) ? 1 : 0;

    return p;
}

GamePadProbe::GamePadProbe(){

    now = 0;

    pinMode(DPAD_LATCH, OUTPUT);
    pinMode(DPAD_CLOCK, OUTPUT);
    pinMode(DPAD_DATA,  INPUT); 

    pinResetFast(DPAD_LATCH);
    pinResetFast(DPAD_CLOCK);
    pinResetFast(DPAD_DATA);
}

void GamePadProbe::query(void){

    pinSetFast(DPAD_LATCH);
    delayMicroseconds(DPAD_WAIT_TIME_US * 2);
    pinResetFast(DPAD_LATCH);

    delayMicroseconds(6);
    now = 0 | (digitalRead(DPAD_DATA) << 0);

    for (register unsigned short i = 1; i < 16; i++) {
        
        pinSetFast(DPAD_CLOCK);
        delayMicroseconds(DPAD_WAIT_TIME_US);

        now |= (digitalRead(DPAD_DATA) << i);

        pinResetFast(DPAD_CLOCK);
        delayMicroseconds(DPAD_WAIT_TIME_US);
    }

    now = ~now;
}