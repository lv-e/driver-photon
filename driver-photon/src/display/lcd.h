#pragma once
#include "pins.h"
#include "geometry.h"

const unsigned short lcd_width     = 130;
const unsigned short lcd_height    = 130;
const unsigned short lcd_pixels    = lcd_width * lcd_height;

class LCD {

    public:

        LCD(
            Region region = Region(lcd_width, lcd_height),
            Pins pins = Pins()
        );
        
        void setup();
        void loop();

        void clear(unsigned int color);

        ~LCD(void);

    private:

        Region _drawRegion;
        Pins _pins;
        bool _ready;
        unsigned short _framebuffer[lcd_pixels];

        void configureSPI();
        void configureGPIO();
        void configureInterrupts();
        void configureDrawRegion();

        void writeOnRegister(unsigned char index, unsigned int data);
        void writeTupleOnRegister(unsigned char index, unsigned char A, unsigned char B);

        void sendResetCommand();
        void sendStartupSequence();
};