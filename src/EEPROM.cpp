#include "EEPROM.h"

void EEPROM::writeBuffer(unsigned int address, byte* buffer, byte bufferSize) {
    setAddress(address, false);
    // for (int i = 0; i < bufferSize; i++) {
    //     Serial.print(buffer[i]);
    //     Serial.print(" ");
    //     Serial.println(buffer[i], HEX);
    // }
    for (unsigned int bufferIndex = 0; bufferIndex < bufferSize;
         bufferIndex++) {
        writeEEPROM(address++, buffer[bufferIndex]);
    }
}

void EEPROM::readToBuffer(unsigned int startAddress, byte* buffer, byte bufferSize) {
    for (unsigned int offset = 0; offset < bufferSize; offset += 1) {
        buffer[offset] = readEEPROM(startAddress + offset);
    }
}

void EEPROM::setup() {
    pinMode(shiftDataPin, OUTPUT);
    pinMode(shiftClkPin, OUTPUT);
    pinMode(shiftLatchPin, OUTPUT);
    digitalWrite(eepromWriteEn, HIGH);
    pinMode(eepromWriteEn, OUTPUT);
}

/*
 * Write a byte to the EEPROM at the specified address.
 */
void EEPROM::writeEEPROM(unsigned int address, byte data) {
    initIoOutput();
    // Serial.print("D:"); Serial.print(data, HEX);Serial.print(" ");
    setAddress(address, /*outputEnable*/ false);
    for (int pin = 0; pin <= 7; pin += 1) {
        digitalWrite(eepromIo[pin], data & 1);
        data = data >> 1;
    }
    digitalWrite(eepromWriteEn, LOW);
    delayMicroseconds(10);
    digitalWrite(eepromWriteEn, HIGH);
    delay(10);

    //DEBUG check
    // byte check = readEEPROM(address);
    // Serial.print("CH:");
    // Serial.print(check, HEX);
    // Serial.print(" ");
    // Serial.println(check);
}

byte EEPROM::readEEPROM(unsigned int address) {
    initIoInput();
    setAddress(address, /*outputEnable*/ true);

    byte data = 0;
    for (int pin = 7; pin >= 0; pin -= 1) {
        data = (data << 1) + digitalRead(eepromIo[pin]);
    }
    return data;
}

/*
 * Output the address bits and outputEnable signal using shift registers.
 */

void EEPROM::setAddress(unsigned int address, bool outputEnable) {
    shiftOut(shiftDataPin, shiftClkPin, MSBFIRST,
             (address >> 8) | (outputEnable ? 0x00 : 0x80));
    shiftOut(shiftDataPin, shiftClkPin, MSBFIRST, address);

    digitalWrite(shiftLatchPin, LOW);
    digitalWrite(shiftLatchPin, HIGH);
    digitalWrite(shiftLatchPin, LOW);
}

void EEPROM::initIoPins(uint8_t mode) {
    for (int pin = 0; pin <= 7; pin += 1) {
        pinMode(eepromIo[pin], mode);
    }
}

void EEPROM::initIoInput() {
    if (ioPinMode != INPUT) {
        initIoPins(INPUT);
        ioPinMode = INPUT;
    }
}

void EEPROM::initIoOutput() {
    if (ioPinMode != OUTPUT) {
        initIoPins(OUTPUT);
        ioPinMode = OUTPUT;
    }
}

