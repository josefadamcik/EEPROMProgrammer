#include <Arduino.h>
#include "EEPROM.h"
#include "SerialReader.h"

byte buffer[bufferSize];
EEPROM eeprom;
SerialReader serialReader;

/*
 * Read the contents of the EEPROM and print them to the serial monitor.
 */
void dumpSegment(unsigned int startAddress) {
    unsigned int endAddress = startAddress + 0xFF;
    for (unsigned int base = startAddress; base <= endAddress; base += 16) {
        byte data[16];
        eeprom.readToBuffer(base, data, 16);

        char buf[80];
        sprintf(buf,
                "%04x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x "
                "%02x %02x %02x %02x %02x %02x",
                base, data[0], data[1], data[2], data[3], data[4], data[5],
                data[6], data[7], data[8], data[9], data[10], data[11],
                data[12], data[13], data[14], data[15]);

        Serial.println(buf);
    }
    Serial.println("=DUMPDONE");
}

void printHelp() {
    Serial.println();
    Serial.println("======");
    Serial.println("EEPROM PROGRAMMER");
    Serial.println("h - print this help");
    Serial.println(
        "dXY - dumps human readable contents of a segment of the memory, XY is "
        "HEX adresses XY00-XYFF");
    Serial.println(
        "wABCD - starts writing mode, ABCD is starting hex address. That will "
        "wait for 16 bytes of input to write to the adress");
    Serial.println("======");
}


void processWriteFromSerial() {
    Serial.print("w");
    unsigned int address = serialReader.readAddressFromSerial();
    if (address > maxAddress - 0xF) {
        Serial.println("=E:Address out of range");
        return;
    }
    Serial.println("=OK:expectingData");

    serialReader.readHexFromSerialToBuffer(buffer, bufferSize);

    Serial.println();
    Serial.println("=DATAOK");
    // buffer is full, write it

    Serial.print("=WRITING:");
    Serial.print(address, HEX);
    Serial.println();
    eeprom.writeBuffer(address, buffer, bufferSize);
    Serial.println();
    Serial.println("=DONE");
}

void processDump() {
    Serial.print("d");
    unsigned int address = serialReader.readHalfAddressFromSerial();
    if (address > maxAddress - 0xFF) {
        Serial.println("=E:Address out of range");
        return;
    }
    Serial.println();
    dumpSegment(address);
}

void setup() {
    // put your setup code here, to run once:
    eeprom.setup();

    Serial.begin(57600);
    printHelp();
}

void loop() {
    if (Serial.available() > 0) {
        byte incomingByte = Serial.read();
        switch (incomingByte) {
            case 'd':
                processDump();
                break;
            case 'w':
                processWriteFromSerial();
            case 'h':
                printHelp();
        }
    }
}
