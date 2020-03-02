

/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.
*/
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

// sensor stuff
#include <Wire.h>

#include <SparkFunBME280.h>
BME280 mySensor;

File myFile;

int logId = 0;

// RTC stuff
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");


  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("values.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to values.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening values.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("values.txt");
  if (myFile) {
    Serial.println("values.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening values.txt");
  }


  // RTC error check
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Adjust the time because the battery sucks
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // I2C setup settings
  mySensor.settings.commInterface = I2C_MODE;
  mySensor.settings.I2CAddress = 0x76;

  // Sensor error check
  Wire.begin();

  if (mySensor.beginI2C() == false) //Begin communication over I2C
  {
    Serial.println("The sensor did not respond. Please check wiring.");
    while (1); //Freeze
  }

}

void loop() {

  // Print date and time
  DateTime now = rtc.now();

  Serial.print(logId);
  Serial.print(", ");

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print(", ");

  //Serial.print(mySensor.readTempC(), 0);
  Serial.print("Temperature: ");
  Serial.print(mySensor.readTempF(), 0);
  Serial.print(", ");

  // If the sensor loaded ok, write:
  Serial.print("Pressure: ");
  Serial.print(mySensor.readFloatPressure(), 1);



  myFile = SD.open("values.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print(logId);
    myFile.print(",");

    myFile.print(now.year(), DEC);
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print(" ");
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    myFile.print(now.second(), DEC);
    myFile.print(",");

    //Serial.print(mySensor.readTempC(), 0);
    myFile.print(mySensor.readTempF(), 0);
    myFile.print(",");

    // If the sensor loaded ok, write:
    myFile.print(mySensor.readFloatPressure(), 1);
    myFile.println();

    // close the file:
    myFile.close();
    Serial.println(" ");
    Serial.println("Above data has been saved!");
  } else {
    // if the file didn't open, print an error:
    Serial.println("Error! Could not open values.txt");
  }


  Serial.println();


  // Delay for five sec and add 5 to timevalue
  delay(1000);
  logId += 1;
}
