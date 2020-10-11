
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

	typedef unsigned char Byte;
	typedef unsigned short ui16;
	typedef unsigned int ui32;

	typedef struct PlanesWalker {

		ui32 byteStart;
        const Byte *const image;
		ui32 sizeInBytes;
		
		ui32 cursor;
		ui32 currentByte;
		ui32 buffer;

		ui32 rleBuffer;
		ui32 currentColorBit;

		PlanesWalker(ui32 _byteStart, const Byte *const _image) : byteStart(_byteStart), image(_image){
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

		Byte planes;
		Byte available;

		ui16 width;
		ui16 height;

        const Byte *const image;
        PlanesWalker *p;

		File(const Byte *const _image) : image(_image){

			Byte headerHigh = *(image + 0);
			Byte headerLow  = *(image + 1);

			available = (headerHigh & 0b11111000) >> 3 == PRIPS_FILE_SIGNATURE;
			available = available && (headerLow & 0b11000000) >> 6 == PRIPS_FILE_VERSION;

			if (!available) return;

			planes 	= (Byte) (headerHigh & 0b00000111);	
			width 	= pow(2, 2 + ((Byte) (headerLow & 0b00111000) >> 3));
			height 	= pow(2, 2 + ((Byte) (headerLow & 0b00000111)));

            p = NULL;
		}

		inline ui32 planesStart(){
			return 2 + pow(2, planes); // 2 bytes header + (n * colors)
		}

		unsigned short color(ui16 index) {
			return *(image + (2 + index)); // 2 bytes header + (n * colors)
		}

		Byte* decompress(){
			
			if (!available) return NULL;
			Byte *buffer = (Byte*) malloc(height * width * sizeof(Byte));
			ui32 planePadding = 0;

			for(ui16 pln = 0; pln < planes; pln++){

				p = new PlanesWalker(planesStart() + planePadding, (const Byte *const) image);
				planePadding += p->sizeInBytes;

				for (ui16 y = 0; y < height; y++) {
					for (ui16 x = 0; x < width; x++) {
						Byte b = pln ? *(buffer + (x + y * width)) : 0;
						b = (b <<= 1) | p->nextPixelColor();
						*(buffer + (x + y * width)) = b;
					}
				}

				delete p;
				p == NULL;
			}

			for (ui16 y = 0; y < height; y++) {
				for (ui16 x = 0; x < width; x++) {
					Byte b = *(buffer + (x + y * width));
					*(buffer + (x + y * width)) = color(b);
				}
			}

			return buffer;
		}

	} File; 
}

#endif

unsigned int frame;

const unsigned char rleImage[] = {0x64,0x6d,0x13,0x1,0xa,0xc,0x15,0x7,0x17,0xe,0x9,0x6,0x5,0x4,0x3,0x11,0x0,0x0,0x0,0xa7,0x80,0x0,0x4,0xef,0x1,0x70,0x14,0xc1,0x60,0x7,0x5a,0xee,0xee,0xee,0xee,0xee,0x5,0x47,0x77,0x77,0x77,0x77,0x77,0x40,0x52,0x98,0xa2,0xcc,0x28,0xb8,0x14,0xaf,0x4d,0x1f,0x44,0x46,0xd3,0xe0,0x1,0x50,0xb2,0x22,0x1,0xd0,0xb5,0x28,0x1c,0x4b,0x24,0x80,0x66,0x8a,0x22,0x22,0x20,0x6e,0x4,0x78,0xb2,0x32,0x36,0x19,0x81,0x1e,0x35,0x90,0x84,0x41,0x78,0x11,0xe3,0xd2,0x52,0x50,0x1d,0x92,0x49,0x46,0x8a,0xc0,0xb0,0xb6,0x47,0xb9,0x38,0xd2,0xb2,0xbb,0xb9,0x65,0x45,0xbb,0xc9,0x49,0x9c,0x28,0xea,0xcc,0x54,0x22,0x56,0x56,0x56,0x50,0x23,0x54,0x48,0x2d,0x2,0x35,0x44,0x82,0xd0,0x23,0x54,0x48,0x2d,0x2,0x35,0x44,0x82,0xd0,0x23,0x54,0x49,0x79,0x59,0x59,0x59,0x59,0x40,0x8d,0x51,0x25,0xe5,0x65,0x65,0x65,0x65,0x2,0x35,0x44,0x2,0xe0,0x23,0x54,0xf0,0x5d,0x2,0x2d,0x4e,0x81,0x62,0x4,0xc,0xac,0xac,0xac,0xad,0x53,0x28,0x30,0xf,0x95,0x95,0x95,0x95,0x97,0x94,0x80,0xc0,0x3e,0xb,0x48,0xc,0x3,0xe0,0xb4,0x84,0xbb,0xbb,0xbb,0xbb,0xbb,0xb0,0xf8,0x2d,0x25,0xdd,0xdd,0xdd,0xdd,0xdd,0xd4,0x3e,0xb,0x49,0x82,0xcc,0x3e,0x56,0x56,0x56,0x56,0x5e,0x52,0x6e,0x44,0x26,0x44,0x26,0x44,0xb0,0xf9,0x59,0x59,0x59,0x59,0x79,0x4b,0x98,0x83,0x8,0x30,0x9c,0x3c,0xb,0x8b,0x98,0x84,0x9c,0x84,0x9c,0xb0,0xf4,0x17,0x49,0x9d,0xe,0x43,0x97,0xf,0x20,0x58,0x9e,0xa2,0x11,0x62,0x11,0x62,0x30,0xf0,0x30,0x78,0x8a,0x42,0xe9,0xb,0xa5,0xe,0x83,0x7,0x82,0xcc,0x3a,0xc,0x1e,0xb,0x30,0xe9,0x2e,0xee,0xee,0xee,0xee,0xed,0xd8,0x99,0x10,0x99,0x10,0x95,0xe,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0x49,0x63,0x8,0x30,0x82,0x70,0xee,0xb,0x32,0x59,0x27,0x21,0x27,0x21,0x2f,0xf,0x6e,0x44,0x26,0x44,0x26,0x44,0xb2,0x50,0xe4,0x39,0x9,0xc3,0xf9,0x88,0x30,0x83,0x9,0xde,0xd1,0x62,0x11,0x62,0x11,0x54,0x3f,0x98,0x84,0x9c,0x84,0x9c,0xb8,0x0,0xf4,0x80,0x0,0xf,0xd5,0x65,0x65,0x60,0x6d,0xe5,0xe5,0xe0,0x6d,0x77,0x77,0x60,0x6d,0x77,0x77,0x60,0x6b,0xd9,0x59,0x58,0x1b,0x5d,0xd9,0x58,0x1b,0x5d,0xd9,0x58,0x3,0xd7,0xbb,0xbb,0xbb,0xbb,0xbb,0x81,0x54,0xc5,0x16,0x61,0x45,0x81,0x51,0xe7,0x3e,0x20,0xd9,0xa0,0x4,0x45,0x20,0x1e,0x84,0x1,0xdc,0x50,0x1d,0x46,0x1,0xcc,0x70,0x1c,0x87,0x1,0xc4,0x20,0x7,0x0,0x80,0x1b,0x82,0x80,0x90,0x80,0x42,0xc,0x2,0x1,0x0,0xfc,0x34,0x1e,0x14,0xf,0x43,0x81,0xc1,0x80,0xec,0x3c,0x1a,0x1c,0xe,0x83,0xc1,0xa1,0xc0,0xe4,0x10,0x6,0x2,0x0,0xe0,0x10,0x6,0x2,0x0,0x98,0x83,0x1,0x21,0x8,0x30,0x28,0x17,0x62,0x4,0x8,0x6,0x2,0x3,0x1,0x42,0x98,0x51,0x81,0x91,0x83,0x42,0x59,0x84,0x21,0xa4,0x6,0x84,0xe,0x18,0xe8,0x34,0x81,0xb2,0xf,0x30,0x4a,0x81,0xb2,0xf,0x20,0x2,0x3a,0xe8,0x19,0xe8,0x8,0x9d,0x41,0x8f,0x50,0x21,0xf3,0xad,0x11,0x10,0x3b,0xc0,0x93,0x2d,0x63,0x83,0xd0,0x24,0x44,0x4,0x43,0xb0,0x24,0x4e,0x12,0xe,0xc0,0x8e,0x1c,0xd,0x1c,0x8,0x21,0xe1,0xa7,0x20,0xb0,0x7a,0x8,0x5,0xa3,0xe3,0xc1,0xc8,0x56,0x68,0x40,0x50,0x1a,0x6,0xc4,0xa0,0x2c,0x28,0xcb,0x93,0xec,0xac,0xac,0xae,0xee,0x59,0x51,0x6e,0xf2,0x52,0x67,0xa,0x3a,0xb3,0x5d,0x27,0x49,0xdd,0xdd,0xdd,0xc0,0x8c,0x41,0x3e,0x5e,0x5e,0x5e,0x5e,0x5f,0x2,0x31,0x4,0xdd,0xdd,0xdd,0xdd,0xdd,0xdc,0x8,0xc4,0x13,0x77,0x77,0x77,0x77,0x77,0x70,0x23,0x10,0x4d,0x95,0x95,0x95,0x95,0x95,0xc0,0x8c,0x41,0x37,0xb6,0x57,0x77,0x77,0x77,0x2,0x31,0x4,0xde,0xd9,0x5d,0xdd,0xdd,0xdc,0x8,0xc4,0x10,0xb,0x80,0x8c,0x43,0x83,0x0,0x22,0x90,0xe0,0xc0,0x10,0x32,0xb2,0xb2,0xb2,0xb4,0x62,0x83,0x0,0xfd,0xdd,0xdd,0xdd,0xdd,0xed,0xc8,0xc,0x3,0xfe,0x5e,0x5e,0x5e,0x5e,0x5f,0x20,0x30,0xf,0xdd,0xdd,0xdd,0xdd,0xdd,0xdc,0x80,0xc0,0x3f,0x77,0x77,0x77,0x77,0x77,0x72,0x12,0xee,0xee,0xee,0xee,0xee,0xc3,0xf6,0x56,0x56,0x56,0x56,0x57,0x22,0x5,0x88,0x3f,0x77,0x77,0x77,0x77,0x7b,0x72,0x27,0x22,0x13,0x22,0x13,0x22,0x48,0x3f,0x77,0x77,0x77,0x77,0x7b,0x72,0xa3,0x10,0x61,0x6,0x12,0x83,0xc0,0xb8,0xa8,0xc4,0x24,0xe4,0x24,0xe6,0xf,0x3,0x2,0x23,0xa1,0xc8,0x72,0xa0,0xf0,0x30,0x69,0x44,0x22,0xc4,0x22,0xc4,0x20,0xf0,0x30,0x68,0x45,0x21,0x74,0x85,0xd3,0xe,0x83,0x6,0x81,0x62,0xe,0x83,0x6,0x81,0x62,0xe,0x83,0x6,0x41,0x32,0x21,0x32,0x21,0x2c,0x3a,0x4b,0xbb,0xbb,0xbb,0xbb,0xbb,0x21,0x6,0x10,0x61,0x4,0xa0,0xea,0x5,0x88,0x84,0x24,0xe4,0x24,0xe4,0x25,0xa0,0xf2,0x72,0x21,0x32,0x21,0x32,0x24,0x88,0xc7,0x21,0xc8,0x4a,0xf,0xa3,0x10,0x61,0x6,0x12,0x9a,0x68,0xb1,0x8,0xb1,0x8,0xac,0x3e,0x8c,0x42,0x4e,0x42,0x4e,0x68,0x1,0xbc,0x0,0xd,0x58,0x20,0x1e,0x84,0x1,0xdc,0x50,0x1d,0x46,0x1,0xd2,0xca,0x7,0x69,0x58,0x1d,0xe0,0x7f,0x81,0xfe,0x69,0xe0,0x70,0x80,0x85,0x60,0x7d,0x60,0x16,0x8,0x2c,0x81,0x48,0x17,0x1,0x48,0x17,0x1,0x49,0xdd,0xdd,0xdd,0xdd,0xdd,0xd0,0x5,0x69,0x8d,0x20,0xc4,0x5,0xdb,0x42,0xa4,0x8a,0xb8,0x0,0x4d,0xfb,0x40,0xcf,0x40,0x44,0xea,0xc,0x7a,0x81,0xf,0x9d,0x60,0xa7,0x81,0x26,0x5a,0x82,0xd8,0x12,0x22,0x26,0xa8,0x23,0x81,0x22,0x79,0xa4,0x82,0x28,0x12,0x4,0x95,0x11,0x2,0x3,0x1,0x8,0x51,0x59,0x18,0x74,0x50,0x3e,0x18,0x4a,0x92,0xa1,0xa1,0xc0,0xe8,0x73,0xe2,0x20,0x72,0x49,0x6b,0x40,0x76,0x75,0x60,0x3e,0x90,0x1f,0xe0,0x24,0x27,0x9e,0x79,0xe0,0x4d,0x81,0xfd,0x1,0xfe,0x3,0xfa,0x7,0xf4,0x26,0xb,0x20,0x48,0x89,0x5,0xc0,0x52,0x5,0xc0,0x52,0x77,0x77,0x77,0x77,0x77,0x74,0x2,0x9a,0x79,0xe7,0x9e,0x1,0x0,0x84,0xd0,0xe4,0x39,0x1,0x2a,0x12,0xa8,0xca,0x8c,0xa8,0x4,0x86,0xb,0x21,0x14,0x39,0xe,0x82,0x0,0x5c,0xc,0xa1,0xf0,0x5c,0x5,0x27,0x77,0x77,0x77,0x77,0x77,0x40,0x21,0x90,0xe4,0x39,0x1,0x7b,0x8c,0xa8,0xca,0x81,0x76,0x5,0x44,0x39,0xe,0x42,0x60,0x53,0xa8,0xca,0x8c,0xa8,0xb4,0x39,0xe,0x40,0x56,0x0,0xc0,0x80,0x0,0xc,0xd8,0x20,0x1e,0x91,0x10,0x1d,0xe2,0x20,0x75,0x8a,0x81,0xce,0x32,0x7,0x2d,0x94,0x84,0x60,0x6a,0x6d,0xed,0x2b,0x81,0xa5,0xf2,0xf2,0xf8,0x1a,0x5f,0xff,0xff,0x81,0xa5,0xb5,0x4d,0x53,0x81,0xa5,0xa3,0x23,0x24,0x6,0xa6,0x8c,0xad,0x28,0x1a,0xec,0xae,0xee,0x5,0x30,0x5c,0x5,0x18,0x2e,0x81,0x42,0xb,0xa0,0x50,0x40,0xb1,0x1,0x41,0x2b,0xbb,0xbb,0xbb,0xbb,0xbd,0x1,0x41,0x52,0x49,0x24,0x92,0x49,0x24,0x92,0x49,0x50,0x14,0x1,0x80,0x14,0x1,0x80,0x14,0x4,0xb1,0xa4,0x18,0x8a,0x5,0x1,0x16,0x85,0x49,0x15,0x76,0x5,0x11,0xc,0x69,0x6,0x36,0x0,0x12,0xfc,0xd0,0x33,0xd0,0x11,0x3a,0x83,0x1e,0xa0,0x43,0xe7,0x5a,0x22,0x20,0x77,0x81,0x26,0x5a,0xd1,0x11,0x10,0x7a,0x4,0x88,0x84,0x3a,0x1d,0x81,0x22,0x74,0x3e,0x1d,0x81,0x34,0xa1,0x8,0x75,0x81,0x2d,0x95,0x11,0x12,0x34,0xe2,0x88,0x4,0x96,0x46,0x46,0xe2,0xc5,0x8,0x38,0x11,0x88,0xd6,0x4c,0x82,0xc6,0x1,0x5e,0x95,0x11,0x40,0xec,0x92,0x49,0x3c,0x90,0xfb,0x29,0x65,0x45,0xbb,0xc9,0x49,0x9c,0x2f,0x51,0x4c,0xac,0xac,0xac,0xac,0xac,0xa,0xbc,0xbc,0xbc,0xbc,0xbc,0xbc,0xa,0xbf,0xff,0xff,0xff,0xff,0xfc,0xa,0xad,0x53,0x54,0xd5,0x35,0x4d,0x53,0x2,0x44,0x62,0x32,0x32,0x32,0x32,0x32,0x2,0x4a,0x18,0x8c,0x8c,0x8c,0x8c,0x8c,0x80,0xaa,0xca,0xca,0xca,0xca,0xca,0xc0,0x64,0x45,0x2,0xe0,0x23,0xa1,0x20,0xb8,0xa,0x60,0xb0,0x11,0x32,0xb2,0xb2,0xb2,0xb2,0xb3,0xab,0xbb,0xbb,0xbb,0xbb,0xbc,0x11,0x72,0xf2,0xf2,0xf2,0xf2,0xf3,0xd2,0x49,0x24,0x92,0x49,0x24,0x92,0x49,0x41,0x17,0xff,0xff,0xff,0xff,0xff,0x2,0xab,0x54,0xd5,0x35,0x4d,0x53,0x54,0xc0,0xaa,0x8c,0x8c,0x8c,0x8c,0x8c,0x80,0xae,0x8c,0x8c,0x8c,0x8c,0x9d,0x21,0xe8,0x72,0x1c,0x81,0x23,0x2b,0x2b,0x2b,0x2b,0x7b,0x1d,0x51,0x95,0x19,0x50,0x14,0x20,0xb8,0xa,0x40,0xb8,0x22,0x87,0x21,0xd0,0x42,0xb,0x1,0x9c,0x10,0x2b,0xbb,0xbb,0xbb,0xbb,0xbc,0xa,0x94,0x92,0x49,0x24,0x92,0x49,0x24,0x92,0x50,0x10,0xe8,0x72,0x1c,0x80,0xbd,0xc6,0x54,0x65,0x40,0xbb,0x2,0xa2,0x1c,0x87,0x21,0x30,0x29,0xd4,0x65,0x46,0x54,0x5a,0x1c,0x87,0x20,0x2b,0x0};
Prips::Byte* image = NULL;
unsigned int width;
unsigned int height;

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

	Prips::File *p = new Prips::File((const unsigned char *const) rleImage);
	width = p->width;
	height = p->height;
	image = p->decompress();
	delete(p);

    frame = 0;
}

void scene_main_on_frame() {

	lvDisplay.blit(lv::Region(10, 0, width, height), (const lv::octet *const) image);
	lvDisplay.blit(lv::Region(20, 0, width, height), (const lv::octet *const) image);
	lvDisplay.blit(lv::Region(30, 0, width, height), (const lv::octet *const) image);

	lvDisplay.blit(lv::Region(frame%128 - 128, 0, width, height), (const lv::octet *const) image);
	lvDisplay.blit(lv::Region(frame%128, 0, width, height), (const lv::octet *const) image);

	frame++;
}

void scene_main_on_exit() {
	free(image);
	image = NULL;
}
