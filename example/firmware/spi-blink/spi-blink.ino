/*

 SCP1000 Barometric Pressure Sensor Display
 Shows the output of a Barometric Pressure Sensor on a
 Uses the SPI library. For details on the sensor, see:
 http://www.sparkfun.com/commerce/product_info.php?products_id=8161
 http://www.vti.fi/en/support/obsolete_products/pressure_sensors/
 This sketch adapted from Nathan Seidle's SCP1000 example for PIC:
 http://www.sparkfun.com/datasheets/Sensors/SCP1000-Testing.zip
 Circuit:
 SCP1000 sensor attached to pins 6, 7, 10 - 13:
 DRDY: pin 6
 CSB: pin 7
 MOSI: pin 11
 MISO: pin 12
 SCK: pin 13
 created 31 July 2010
 modified 14 August 2010
 by Tom Igoe
 */

// the sensor communicates using SPI, so include the library:
#include <SPI.h>

// pins used for the connection with the sensor
// the other you need are controlled by the SPI library):
const int dataReadyPin = 6;
const int chipSelectPin = 10;
const int ledPin = A0;

void setup() {
  Serial.begin(9600);
  SPI.begin();

  // initialize the  data ready and chip select pins:
  pinMode(dataReadyPin, INPUT);
  pinMode(chipSelectPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // give the sensor time to set up:
  delay(100);
}

void loop() {
  //Select High Resolution Mode
  uint64_t toSend = 0x01248EDB7FAA5566;
  spiWrite(&toSend, sizeof(toSend));
  digitalWrite(ledPin, LOW);
  _delay_us(200);
}

//Sends a write command to SCP1000
void spiWrite(void *buffer, size_t size) {

  // take the chip select low to select the device:
  digitalWrite(ledPin, HIGH);
  digitalWrite(chipSelectPin, LOW);

  // SCP1000 expects the register address in the upper 6 bits.
  // and the write command in the lower 2 bits. 
  // address = address << 2 | WRITE;

  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

  SPI.transfer(buffer, size);
  // SPI.transfer(data);  //Send value to record into register

  SPI.endTransaction();

  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
}
