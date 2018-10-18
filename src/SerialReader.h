#include <Arduino.h>

#ifndef SERIALREADERINCL
#define SERIALREADERINCL

class SerialReader {
    public:
        unsigned int readAddressFromSerial();
        unsigned int readHalfAddressFromSerial();
        void readHexFromSerialToBuffer(byte* buffer, byte bufferSize);
    private:
        byte hexToNum(byte hex);
};

#endif