#pragma once

#include "pins.h"
#include "geometry.h"
#include "lvk.h"

const unsigned short lcd_width     = lvk_display_w;
const unsigned short lcd_height    = lvk_display_h;
const unsigned short lcd_pixels    = lcd_width * lcd_height;

class LCD {

    public:

        static LCD& shared(){
            static LCD instance;
            return instance;
        }
        
        void setup();
        void loop();

        bool waitingFrame();
        void beginDrawing();
        void endDrawing();

        void clear(unsigned int color);

        ~LCD(void);

    private:

        LCD(
            Region region = Region(lcd_width, lcd_height),
            Pins pins = Pins()
        );

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