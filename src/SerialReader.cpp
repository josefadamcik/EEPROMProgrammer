#include "SerialReader.h"


unsigned int SerialReader::readAddressFromSerial() {
    unsigned int address = 0;
    byte addresDigitCount = 0;
    while (addresDigitCount < 4) {
        while (Serial.available() == 0)
            ;
        byte readByte = hexToNum(Serial.read());
        Serial.print(readByte, HEX);
        address = address << 4;
        address |= readByte;
        addresDigitCount++;
    }
    return address;
}

unsigned int SerialReader::readHalfAddressFromSerial() {
    unsigned int address = 0;
    byte addresDigitCount = 0;
    while (addresDigitCount < 2) {
        while (Serial.available() == 0)
            ;
        byte readByte = hexToNum(Serial.read());
        Serial.print(readByte, HEX);
        address = address << 4;
        address |= readByte;
        addresDigitCount++;
    }
    address =
        address << 8;  // we have read the most significant byte, so shift
    return address;
}

void SerialReader::readHexFromSerialToBuffer(byte* buffer, byte bufferSize) {
    // read data intoBuffer
    byte bufferIndex = 0;
    while (bufferIndex < bufferSize) {
        // read two chars per byte
        byte charIndex = 0;
        byte value = 0;
        while (charIndex < 2) {
            value = value << 4;
            while (Serial.available() == 0)
                ;
            byte readByte = hexToNum(Serial.read());
            Serial.print(readByte, HEX);
            value |= readByte;
            charIndex++;
        }
        // store them in buffuer
        buffer[bufferIndex++] = value;
    }
}

byte SerialReader::hexToNum(byte hex) {
    if (hex >= 'a' && hex <= 'f') {
        return hex - 'a' + 10;
    } else if (hex >= 'A' && hex <= 'F') {
        return hex - 'A' + 10;
    } else if (hex >= '0' && hex <= '9') {
        return hex - '0';
    } else {
        return 0;
    }
}