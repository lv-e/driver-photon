#include "lv-driver/display/lcd.h"
#include "lv-driver/display/diagnostics.h"

#include <Particle.h>

#define USE_SPI_W_DMA

#ifdef USE_OCTAPIXELS
void lvDriver_DrawHLine(lv::half line, lv::OctaPixel* data){
    
    static lv::half pixels[lvk_display_w];
    static lv::OctaPixel octapixel;
    static unsigned int drawn;
    
    drawn = 0;

    for (unsigned int octa = 0; octa <= lvk_octaspixels_per_line; octa++){
        octapixel = *(data+octa);
        for (unsigned int pixel = 0; pixel < 8 && drawn < lvk_display_w; pixel++) {

            switch (pixel) {
                case 0: pixels[drawn] = palette[octapixel.xa]; break;
                case 1: pixels[drawn] = palette[octapixel.xb]; break;
                case 2: pixels[drawn] = palette[octapixel.xc]; break;
                case 3: pixels[drawn] = palette[octapixel.xd]; break; 
                case 4: pixels[drawn] = palette[octapixel.xe]; break;
                case 5: pixels[drawn] = palette[octapixel.xf]; break;
                case 6: pixels[drawn] = palette[octapixel.xg]; break;
                case 7: pixels[drawn] = palette[octapixel.xh]; break;
            }

            drawn++;
        }
    }

    LCD::shared().drawLine(pixels);

}
#else 

#if lvk_measuring == true
unsigned long frame_time;
int fps, free_memory, spi_transfer;
#endif

void lvDriver_DrawHLine(lv::half line, lv::octet (&stream)[lvk_display_w]){

    static unsigned short hLinePixels[lvk_display_w];    
    register lv::word pixelCursor = 0;

    do {
        hLinePixels[pixelCursor] = palette[stream[pixelCursor]];
    } while (pixelCursor++ < lvk_display_w);

    LCD::shared().drawLine(hLinePixels);
}

#if lvk_measuring == true
extern lv::octet lvDriver_CurrentFPS(void){
    return (lv::octet) fps;
}
#endif

#endif

extern "C" {

    volatile bool isrActivated = false;

    void timerISRoutine() {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
        isrActivated = true;
    }  
}

LCD::LCD(Region region, Pins pins) : _drawRegion(region), _pins(pins){
    _ready = false;
}

LCD::~LCD(void) {}

void LCD::setup(){

    if(_ready) return;

    #if lvk_measuring == true
        frame_time  = millis();
        fps         = 0;
        spi_transfer = 0;
        Particle.variable("fps", &fps, INT);
        Particle.variable("spi transfer", &spi_transfer, INT);

        free_memory = DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_FREE_MEMORY);
        Particle.variable("free_memory", &free_memory, INT);

    #endif

    configureSPI();
    sendResetCommand();
    sendStartupSequence();
    configureInterrupts();
    _ready = true;
}

bool LCD::waitingFrame() {
    return isrActivated;
}

void LCD::beginDrawing(){
    if (!isrActivated) return;
    configureDrawRegion();
    _pins.beginTransmission();
    _pins.configureAsData();
}

#ifdef USE_OCTAPIXELS
void LCD::drawLine(unsigned short (&data)[lvk_display_w]){
    SPI.transfer((byte*) data, NULL, lvk_display_w * 2, NULL);
}
#else 
void LCD::drawLine(unsigned short (&data)[lvk_display_w]){
    
    #if lvk_measuring == true
        unsigned long before = micros();
    #endif

    #ifdef USE_SPI_W_DMA
        SPI.transfer((byte*) data, NULL, lvk_display_w * 2, NULL);    
    #else
        for(int i = 0; i < 130; i++) {
            SPI.transfer(data[i] >> 8);
            SPI.transfer(data[i]);
        }
    #endif

    #if lvk_measuring == true
        spi_transfer = (int) (micros() - before);
    #endif
}
#endif

void LCD::endDrawing() {
    _pins.endTransmission();
    isrActivated = false;

    // gather metrics
    #if lvk_measuring == true

        unsigned long now = millis();
        unsigned long diff = now - frame_time;
        fps = (int) abs( 1.0 / (((float) diff)/ 1000.0));
        frame_time = now;

        free_memory = DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_USED_RAM);

    #endif
}

void LCD::configureSPI(){
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0); 
}

void LCD::configureInterrupts(){

    // using NVIC & TIM data structures
    NVIC_InitTypeDef nvicStructure;
    TIM_TimeBaseInitTypeDef timerInitStructure;
    TIM_TypeDef* TIMx;
    TIMx = TIM5;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    // enable timer interrupt
    nvicStructure.NVIC_IRQChannel = TIM5_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 1;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);

    // set the timer frequency
    timerInitStructure.TIM_Prescaler      = 4;
    timerInitStructure.TIM_CounterMode    = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period         = lvk_60hz ? 200000 : 400000;
    timerInitStructure.TIM_ClockDivision  = TIM_CKD_DIV4;
    TIM_TimeBaseInit(TIMx, &timerInitStructure);

    // attach interrupt
    attachSystemInterrupt(SysInterrupt_TIM5_IRQ, timerISRoutine);

    // and run
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIMx, ENABLE);

}

void LCD::sendResetCommand(){
    _pins.resetReset(); delay(100);
    _pins.setReset(); delay(50);
}

void LCD::sendStartupSequence(){

    sendResetCommand(); // Reset before LCD Init.

    writeOnRegister(0x10,0x2F8E); /* power control 1 */
    writeOnRegister(0x11,0x000C); /* power control 2 */
    writeOnRegister(0x07,0x0021); /* display control */
    writeOnRegister(0x28,0x0006); /* vcom OTP */
    writeOnRegister(0x28,0x0005);
    writeOnRegister(0x27,0x057F); /* further bias current setting */
    writeOnRegister(0x29,0x89A1); /* vcom OTP */
    writeOnRegister(0x00,0x0001); /* OSC en */
    delay(100);

    writeOnRegister(0x29,0x80B0); /* vcom OTP */
    delay(30);

    writeOnRegister(0x29,0xFFFE); /* vcom OTP */
    writeOnRegister(0x07,0x0023); /* display control */
    delay(30);

    writeOnRegister(0x07,0x0233); /* display control */
    writeOnRegister(0x01,0x2183); /* driver output control, REV, TB, RGB */
    writeOnRegister(0x03,0x6830); /* entry mode, 65K, ram, ID0 */
    writeOnRegister(0x2F,0xFFFF); /* 2A ~ 2F, test */
    writeOnRegister(0x2C,0x8000);
    writeOnRegister(0x27,0x0570);
    writeOnRegister(0x02,0x0300); /* driving wave form control */
    writeOnRegister(0x0B,0x580C); /* frame cycle control */
    writeOnRegister(0x12,0x0609); /* power control 3 */
    writeOnRegister(0x13,0x3100); /* power control 4 */
    delay(10);
}

void LCD::configureDrawRegion(){

    writeTupleOnRegister(0x44, 
        _drawRegion.origin.x + _drawRegion.size.width + 1,
        _drawRegion.origin.x + 2
    );

    writeTupleOnRegister(0x45, 
        _drawRegion.origin.y + _drawRegion.size.height + 1,
        _drawRegion.origin.y + 2
    );

    writeTupleOnRegister(0x21, 
        _drawRegion.origin.x + 2,
        _drawRegion.origin.y + 2
    );
  
    _pins.beginTransmission();
    _pins.configureAsCommand();
    SPI.transfer(0x22);
    _pins.endTransmission();
}

void LCD::writeOnRegister(unsigned char index, unsigned int data){

    // write index
    _pins.beginTransmission();
    _pins.configureAsCommand();
    SPI.transfer(index);
    _pins.endTransmission();

    // write data
    _pins.beginTransmission();
    _pins.configureAsData();
    SPI.transfer(data >> 8);
    SPI.transfer(data);
    _pins.endTransmission();
}

void LCD::writeTupleOnRegister(unsigned char index, unsigned char A, unsigned char B){

    // write index
    _pins.beginTransmission();
    _pins.configureAsCommand();
    SPI.transfer(index);
    _pins.endTransmission();

    // write data A
    _pins.beginTransmission();
    _pins.configureAsData();
    SPI.transfer(A);
    SPI.transfer(B);
    _pins.endTransmission();
}