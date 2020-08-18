#pragma once

#include "pins.h"
#include "geometry.h"
#include "engine.h"
#include "lvk.h"

const unsigned short lcd_width     = lvk_display_w;
const unsigned short lcd_height    = lvk_display_h;
const unsigned short lcd_pixels    = lcd_width * lcd_height;

static lv::half palette[16] = {

    0b1110000000000111,
    0b0001110000000000,
    0b0000001110000000,
    0b0000000001110000,

    0b0000000000000111,
    0b0111000000111000,
    0b0000000000000000,
    0b0000000000000000,

    0b0001111110000000,
    0b0000000001111111,
    0b0000000000000000,
    0b0111111000000000,
    
    0b0000000111110000,
    0b0011111111000000,
    0b0000000111111100,
    0b0111111111100000,
};

class LCD {

    public:

        static LCD& shared(){
            static LCD instance; return instance;
        }
        
        void setup();
        void loop();

        bool waitingFrame();
        void beginDrawing();
        void drawLine(unsigned short* data);
        void endDrawing();

        ~LCD(void);

    private:

        LCD(
            Region region = Region(lcd_width, lcd_height),
            Pins pins = Pins()
        );

        Region _drawRegion;
        Pins _pins;
        bool _ready;

        void configureSPI();
        void configureGPIO();
        void configureInterrupts();
        void configureDrawRegion();

        void writeOnRegister(unsigned char index, unsigned int data);
        void writeTupleOnRegister(unsigned char index, unsigned char A, unsigned char B);

        void sendResetCommand();
        void sendStartupSequence();
};