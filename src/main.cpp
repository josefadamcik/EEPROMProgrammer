#include <Arduino.h>

static const uint8_t shiftDataPin = 2;
static const uint8_t shiftClkPin = 3;
static const uint8_t shiftLatchPin = 4;
static const uint8_t eepromIo0 = 5;
static const uint8_t eepromIo7 = 12;
static const uint8_t eepromWriteEn = 13;
// #define SHIFT_DATA 2
// #define SHIFT_CLK 3
// #define SHIFT_LATCH 4
// #define EEPROM_D0 5
// #define EEPROM_D7 12
// #define WRITE_EN 13

/*
 * Output the address bits and outputEnable signal using shift registers.
 */
void setAddress(int address, bool outputEnable) {
  shiftOut(shiftDataPin, shiftClkPin, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(shiftDataPin, shiftClkPin, MSBFIRST, address);

  digitalWrite(shiftLatchPin, LOW);
  digitalWrite(shiftLatchPin, HIGH);
  digitalWrite(shiftLatchPin, LOW);
}


/*
 * Read a byte from the EEPROM at the specified address.
 */
byte readEEPROM(int address) {
  for (int pin = eepromIo0; pin <= eepromIo7; pin += 1) {
    pinMode(pin, INPUT);
  }
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
void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  for (int pin = eepromIo0; pin <= eepromIo7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

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
void printContents() {
  for (int base = 0; base <= 255; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}


// 4-bit hex decoder for common anode 7-segment display
byte data[] = { 0x81, 0xcf, 0x92, 0x86, 0xcc, 0xa4, 0xa0, 0x8f, 0x80, 0x84, 0x88, 0xe0, 0xb1, 0xc2, 0xb0, 0xb9 };

// 4-bit hex decoder for common cathode 7-segment display
// byte data[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b, 0x77, 0x1f, 0x4e, 0x3d, 0x4f, 0x47 };

void eraseEEPROM() {
  Serial.print("Erasing EEPROM");
  //TODO: this is not the max address
  for (int address = 0; address <= 2047; address += 1) {
    writeEEPROM(address, 0xff);

    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done");

}

void setup() {
  // put your setup code here, to run once:
  pinMode(shiftDataPin, OUTPUT);
  pinMode(shiftClkPin, OUTPUT);
  pinMode(shiftLatchPin, OUTPUT);
  digitalWrite(eepromWriteEn, HIGH);
  pinMode(eepromWriteEn, OUTPUT);
  
  Serial.begin(57600);
  Serial.println();

  // Erase entire EEPROM
  //eraseEEPROM();


  // Program data bytes

  Serial.print("Programming EEPROM");
  for (int address = 0; address < sizeof(data); address += 1) {
    writeEEPROM(address, data[address]);

    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done");

  // Read and print out the contents of the EERPROM
  Serial.println("Reading EEPROM");
  printContents();
}


void loop() {
  //nop
}
