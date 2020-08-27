#include "lv-driver/display/pins.h"

Pins::Pins(){
    chip_select     = A2;
    data_command    = A1;
    reset           = A0;

    pinMode(chip_select,  OUTPUT);
    pinMode(reset,        OUTPUT);
    pinMode(data_command, OUTPUT); 
    pinMode(D7, OUTPUT); 

    endTransmission();
    configureAsData();
    setReset();
}

