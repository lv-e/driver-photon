
#include "lv-game/scene_main.h"
#include "lv-game/shared.h"
#include "lv-engine/engine.h"
#include <stdlib.h>

#ifndef PRIPS_DECODER
#define PRIPS_DECODER

#define PRIPS_FILE_VERSION 		0b01
#define PRIPS_FILE_SIGNATURE 	0b01100

#include <math.h>

namespace Prips{

	typedef unsigned char byte;
	typedef unsigned short ui16;
	typedef unsigned int ui32;

	typedef struct PlanesWalker {

		ui32 byteStart;
		ui32 sizeInBytes;

        const byte *const image;
		
		ui32 cursor;
		ui32 currentByte;
		ui32 buffer;

		ui32 rleBuffer;
		ui32 currentColorBit;

		PlanesWalker(ui32 _byteStart, const byte *const _image) : byteStart(_byteStart), image(_image){
            rewind();
		}

        void rewind(){

            buffer = 8;
			cursor = byteStart;
			currentByte = *(image + cursor);

			sizeInBytes = advance();
			goToNextByte();
			sizeInBytes += cursor - byteStart;

			pickFirstBit();
			rleBuffer = advance();

        }

		inline ui32 nextPixelColor(){

			if(--rleBuffer == 0){
				rleBuffer = advance();
				currentColorBit = !currentColorBit;
			}

			return currentColorBit;
		}

		inline void pickFirstBit(){
			currentColorBit = (currentByte & 0x80) >> 7;
			currentByte <<= 1;
			buffer--;
		}

		inline void goToNextByte(){
			currentByte = *(image + ++cursor);
			buffer = 8;
		}

		ui32 advance(){

			ui32 bit = 0;
			ui32 number = 0;
			ui32 count = 0;
			ui32 counting = 1;

			do {
				
				bit = (currentByte & 0x80);

				currentByte <<= 1;
				buffer--;

				if(!buffer) goToNextByte();

				if (counting) {
					if(!bit) count++;
					else counting = false;	
				} 

				if (!counting) {
					number |= bit == 0 ? 0 : 1;
					if(count-- == 0) break;
					else number <<= 1;
				}
				
			} while (true);

			return number;
		}

	} PlanesWalker;

	typedef struct File {

		byte planes;
		byte available;
		byte hasAlpha;

		ui16 width;
		ui16 height;

        const byte *const image;
        PlanesWalker *p;

		File(const byte *const _image) : image(_image){

			byte headerHigh = *(image + 0);
			byte headerLow  = *(image + 1);

			available = (headerHigh & 0b11111000) >> 3 == PRIPS_FILE_SIGNATURE;
			available = available && (headerLow & 0b11000000) >> 6 == PRIPS_FILE_VERSION;

			if (!available) return;

			planes 	= (byte) (headerHigh & 0b00000111);	
			width 	= pow(2, 2 + ((byte) (headerLow & 0b00111000) >> 3));
			height 	= pow(2, 2 + ((byte) (headerLow & 0b00000111)));

            p = NULL;
		}

		inline ui32 planesStart(){
			return 2 + pow(2, planes); // 2 bytes header + (n * colors)
		}

		unsigned short color(ui16 index) {
			return *(image + (2 + index)); // 2 bytes header + (n * colors)
		}

		byte* decompress(){
			
			if (!available) return NULL;
			byte *buffer = (byte*) malloc(height * width * sizeof(byte));
			ui32 planePadding = 0;
			hasAlpha = false;

			for(ui16 pln = 0; pln < planes; pln++){

				p = new PlanesWalker(planesStart() + planePadding, (const byte *const) image);
				planePadding += p->sizeInBytes;

				for (ui16 y = 0; y < height; y++) {
					for (ui16 x = 0; x < width; x++) {
						byte b = pln ? *(buffer + (x + y * width)) : 0;
						b = (b <<= 1) | p->nextPixelColor();
						*(buffer + (x + y * width)) = b;
					}
				}

				delete p;
				p == NULL;
			}

			for (ui16 y = 0; y < height; y++) {
				for (ui16 x = 0; x < width; x++) {
					const byte c = color(*(buffer + (x + y * width)));
					*(buffer + (x + y * width)) = c;
					if (c == 0) hasAlpha = true;
				}
			}

			return buffer;
		}

	} File; 

	typedef struct Drawable {
		
		byte width;
		byte height;
		byte hasAlpha;
		lv::octet *decompressed;

		Drawable(const byte *const data) {
			
			File *parser = new File(data);
			
			width = parser->width;
			height = parser->height;
			decompressed = parser->decompress();
			hasAlpha = parser->hasAlpha;

			delete parser;
			parser = NULL;
		}

		void draw(const byte x, const byte y) {
			lvDisplay.blit(lv::Region( x, y, width, height), decompressed);
		}

		~Drawable(){
			free(decompressed);
			decompressed = NULL;
		}

	} Drawable;
}

#endif

unsigned int frame;

namespace PNG {
	const unsigned char rawPenPen[] = {0x63,0x52,0x0,0x1,0x16,0x15,0x14,0x5,0x1b,0x0,0x1e,0x1,0x23,0x82,0xd6,0x35,0xd,0xb8,0xa9,0x61,0x99,0x62,0xae,0x18,0x2d,0xb0,0xb,0x0,0x1,0x15,0xb4,0x21,0x5e,0x21,0x5e,0x21,0xb4,0x20,0x40,0xb6,0x30,0xa2,0x33,0x9e,0x8a,0x64,0xb7,0x85,0xe5,0x6c,0xe,0x80,0x4,0xf3,0x1c,0x59,0x85,0x15,0xd7,0xe4,0x18,0x41,0x84,0x18,0x47,0x49,0x65,0x25,0xe5,0x24,0xbb,0x71,0x49,0xf2,0xd2,0xa4,0x9a,0x4c,0x61,0x72,0xba};
	Prips::Drawable *penpen;
}

void scene_main_setup() {
    scenesTable[SCENE_MAIN] = {
        &scene_main_on_awake,
        &scene_main_on_enter,
        &scene_main_on_frame,
        &scene_main_on_exit
    };
}

void scene_main_on_awake() {

}

void scene_main_on_enter() {
	PNG::penpen = new Prips::Drawable( (const Prips::byte *const) &PNG::rawPenPen);
    frame = 0;
}

void scene_main_on_frame() {
	lvDisplay.clear(0);
	PNG::penpen->draw(20, 20);
	frame += 1;
}

void scene_main_on_exit() {
	delete PNG::penpen;
	PNG::penpen = nil;
}
