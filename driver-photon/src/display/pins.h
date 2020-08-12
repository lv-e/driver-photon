
#pragma once
#include "Particle.h"

typedef struct Pins {

    unsigned short chip_select;
    unsigned short data_command;
    unsigned short reset;

    Pins();

    inline void beginTransmission(void) { pinResetFast(chip_select); }
    inline void endTransmission(void) { pinSetFast(chip_select); }

    inline void configureAsData(void) { pinSetFast(data_command); }
    inline void configureAsCommand(void) { pinResetFast(data_command); }

    inline void setReset(void) { pinSetFast(reset); }
    inline void resetReset(void) { pinResetFast(reset); }
    
} Pins;
