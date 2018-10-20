#include <Arduino.h>

#ifndef EEPROMINCL 
#define EEPROMINCL

// Ben Eater's layout
// static const uint8_t shiftDataPin = 2; //SER
// static const uint8_t shiftClkPin = 3; //SRCLK
// static const uint8_t shiftLatchPin = 4; //RCLK
// static const uint8_t eepromIo[] = { 5, 6, 7, 8, 9, 10, 11, 12};
// static const uint8_t eepromWriteEn = 13;

//Using analog pins in order to make a shield for Arduino UNO 
//In order to be able to fit pin headers we are using PINS: 8-13+gnd and A0-05 (+vcc,gnd)
static const uint8_t shiftDataPin = 8; //SER
static const uint8_t shiftClkPin = 10; //SRCLK
static const uint8_t shiftLatchPin = 9; //RCLK
static const uint8_t eepromIo[] = { 11, 12, 13, A0, A1, A2, A3, A4}; //LEFT SIDEY: 2,1,0; RIGHT SIDE, 3,4,5,6,7
static const uint8_t eepromWriteEn = A5;

static const uint8_t bufferSize = 16;
static const unsigned int maxAddress = 0x7FFF;

class EEPROM {
    public:
       EEPROM() {};
       void setup();
       void writeBuffer(unsigned int address, byte* buffer, byte bufferSize);
       void readToBuffer(unsigned int startAddress, byte* buffer, byte bufferSize);
    private:
        uint8_t ioPinMode;
        void setAddress(unsigned int address, bool outputEnable);
        void initIoPins(uint8_t mode);
        void initIoInput();
        void initIoOutput();
        byte readEEPROM(unsigned int address);
        void writeEEPROM(unsigned int address, byte data);
};

#endif