#include <Arduino.h>

static const uint8_t shiftDataPin = 2;
static const uint8_t shiftClkPin = 3;
static const uint8_t shiftLatchPin = 4;
static const uint8_t eepromIo0 = 5;
static const uint8_t eepromIo7 = 12;
static const uint8_t eepromWriteEn = 13;
static const uint8_t bufferSize = 16;
static const unsigned int maxAddress = 0x7FFF;
byte buffer[bufferSize];

/*
 * Output the address bits and outputEnable signal using shift registers.
 */
void setAddress(unsigned int address, bool outputEnable) {
  shiftOut(shiftDataPin, shiftClkPin, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(shiftDataPin, shiftClkPin, MSBFIRST, address);

  digitalWrite(shiftLatchPin, LOW);
  digitalWrite(shiftLatchPin, HIGH);
  digitalWrite(shiftLatchPin, LOW);
}


void initIoPins(uint8_t mode) {
  for (int pin = eepromIo0; pin <= eepromIo7; pin += 1) {
    pinMode(pin, mode);
  }
}

void initIoInput() {
  initIoPins(INPUT);
}

void initIoOutput() {
  initIoPins(OUTPUT);
}

/*
 * Read a byte from the EEPROM at the specified address.
 */
byte readEEPROM(unsigned int address) {
  initIoInput();
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (int pin = eepromIo7; pin >= eepromIo0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}

/*
 * Write a byte to the EEPROM at the specified address.
 */
void writeEEPROM(unsigned int address, byte data) {
  initIoOutput();
  setAddress(address, /*outputEnable*/ false);
  for (int pin = eepromIo0; pin <= eepromIo7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }
  digitalWrite(eepromWriteEn, LOW);
  delayMicroseconds(1);
  digitalWrite(eepromWriteEn, HIGH);
  delay(10);
}


/*
 * Read the contents of the EEPROM and print them to the serial monitor.
 */
void dumpContents() {
  for (unsigned int base = 0; base <= maxAddress; base += 16) {
    byte data[16];
    for (unsigned int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%04x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
  Serial.println("=DUMPDONE");
}

void eraseEEPROM() {
  Serial.print("Erasing EEPROM");
  //TODO: this is not the max address
  for (unsigned int address = 0; address <= 2047; address += 1) {
    writeEEPROM(address, 0xff);

    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done");

}

void printHelp() {
  Serial.println();
  Serial.println("======");
  Serial.println("EEPROM PROGRAMMER");
  Serial.println("h - print this help");
  Serial.println("d - dumps human readable contents of the whole memory");
  Serial.println("wABCD - starts writing mode, ABCD is starting hex address. That will wait for 16 bytes of input to write to the adress");
  Serial.println("======");
}



int hexToNum(byte hex) {
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

unsigned int readAddressFromSerial() {
  unsigned int address = 0;
  byte addresDigitCount = 0;
  while (addresDigitCount < 4) {
      while (Serial.available() == 0);
      byte readByte = hexToNum(Serial.read());
      Serial.print(readByte, HEX);
      address = address << 4;
      address |= readByte;
      addresDigitCount++;
  }
  return address;
} 

void writeFromSerial() {
  Serial.print("w");
  unsigned int address = readAddressFromSerial();
  //read adress
  if (address > maxAddress - 15) {
    Serial.println("=E:Address out of range");
    return;
  }
  Serial.println("=OK:expectingData");
  
  //read data intoBuffer
  byte bufferIndex = 0;
  while (bufferIndex < bufferSize) {
      //read two chars per byte
      byte charIndex = 0;
      byte value = 0;
      while  (charIndex < 2) {
          value = value << 4;
          while (Serial.available() == 0);
          byte readByte = hexToNum(Serial.read());
          Serial.print(readByte, HEX);
          value |= readByte;
          charIndex++;
      }
      //store them in buffuer
      buffer[bufferIndex++] = value;
  }
  Serial.println();
  Serial.println("=DATAOK");
  //buffer is full, write it
  setAddress(address, false);
  Serial.print("=WRITING:");
  Serial.print(address, HEX);
  Serial.println();
  for (bufferIndex = 0; bufferIndex < bufferSize; bufferIndex++) {
    Serial.print(buffer[bufferIndex], HEX);
    writeEEPROM(address++, buffer[bufferIndex]);
  }
  Serial.println();
  Serial.println("=DONE");
}

void setup() {
  // put your setup code here, to run once:
  pinMode(shiftDataPin, OUTPUT);
  pinMode(shiftClkPin, OUTPUT);
  pinMode(shiftLatchPin, OUTPUT);
  digitalWrite(eepromWriteEn, HIGH);
  pinMode(eepromWriteEn, OUTPUT);
  
  Serial.begin(57600);
  printHelp();
}

void loop() {
  
  if (Serial.available() > 0) {
    byte incomingByte = Serial.read();
    switch(incomingByte) {
      case 'd':
        dumpContents();
        break;
      case 'w':
        writeFromSerial();
      case 'h':
        printHelp();
    }
  }
}
