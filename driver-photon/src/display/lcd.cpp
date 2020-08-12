#include "lcd.h"
#include "Particle.h"

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
    configureSPI();
    sendResetCommand();
    sendStartupSequence();
    configureInterrupts();
    _ready = true;
}

void LCD::loop(){
    if (!isrActivated) return;
    isrActivated = false;

    SINGLE_THREADED_BLOCK(){
        _pins.beginTransmission();
        _pins.configureAsData();
        SPI.transfer((void*) _framebuffer, NULL, lcd_pixels * 2, NULL);
        _pins.endTransmission();
    }
}

void LCD::clear(unsigned int color){
    unsigned int i = 0;
    while(i++ < lcd_pixels) _framebuffer[i] = color;
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
    timerInitStructure.TIM_Prescaler      = 1;
    timerInitStructure.TIM_CounterMode    = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period         = 1200;
    timerInitStructure.TIM_ClockDivision  = TIM_CKD_DIV4;
    TIM_TimeBaseInit(TIMx, &timerInitStructure);

    // attach interrupt
    attachSystemInterrupt(SysInterrupt_TIM5_IRQ, timerISRoutine);

    // make it go
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIMx, ENABLE);

    clear(0x00);
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

    writeTupleOnRegister(0x44, 
        _drawRegion.origin.x + _drawRegion.size.width + 2,
        _drawRegion.origin.x + 2
    );

    writeTupleOnRegister(0x45, 
        _drawRegion.origin.y + _drawRegion.size.height + 2,
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
    _pins.endTransmission();

    // write data B
    _pins.beginTransmission();
    _pins.configureAsData();
    SPI.transfer(B);
    _pins.endTransmission();
}