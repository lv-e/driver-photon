#pragma once

#include "lv-driver/display/pins.h"
#include "lv-driver/display/geometry.h"
#include "lv-engine/engine.h"
#include "lv-game/lvk.h"

const unsigned short lcd_width     = lvk_display_w;
const unsigned short lcd_height    = lvk_display_h;
const unsigned short lcd_pixels    = lcd_width * lcd_height;

static lv::half palette[32] = {

	0b0000000000000000,
	0b0000011000100001,
	0b0100011101000001,
	0b1100011001100001,
	0b1010011110001010,
	0b1000010011011011,
	0b0000110011011101,
	0b0001001111101110,
	0b1000011011111111,
	0b0010101010011111,
	0b1110011001101101,
	0b1010110100110100,
	0b0100010101001011,
	0b0100010001010010,
	0b1110011100110001,
	0b1110111000111001,
	0b0001000000110011,
	0b0111110001011011,
	0b1101000000110010,
	0b0111110001011110,
	0b1101111111001110,
	0b1111111111111111,
	0b0111011010011101,
	0b1111000010000011,
	0b0100110101101011,
	0b1010101001011010,
	0b0001000101110010,
	0b1000011010101001,
	0b1010110011011010,
	0b1101011111010011,
	0b1010100110001100,
	0b0110011010001011
};

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
		
        void drawLine(unsigned short (&data)[lvk_display_w]);
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