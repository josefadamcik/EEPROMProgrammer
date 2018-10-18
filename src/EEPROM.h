#include <Arduino.h>

#ifndef EEPROMINCL 
#define EEPROMINCL

static const uint8_t shiftDataPin = 2;
static const uint8_t shiftClkPin = 3;
static const uint8_t shiftLatchPin = 4;
static const uint8_t eepromIo0 = 5;
static const uint8_t eepromIo7 = 12;
static const uint8_t eepromWriteEn = 13;
static const uint8_t bufferSize = 16;\
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