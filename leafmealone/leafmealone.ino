/*************************************************** 
  This is a library for the CAP1188 I2C/SPI 8-chan Capacitive Sensor

  Designed specifically to work with the CAP1188 sensor from Adafruit
  ----> https://www.adafruit.com/products/1602

  These sensors use I2C/SPI to communicate, 2+ pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 /*************************************************** 
  This is an example for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);



// Cap1188 libraries
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CAP1188.h>

// Reset Pin is used for I2C or SPI
#define CAP1188_RESET  9

// CS pin is used for software or hardware SPI
#define CAP1188_CS  10

// These are defined for software SPI, for hardware SPI, check your 
// board's SPI pins in the Arduino documentation
#define CAP1188_MOSI  11
#define CAP1188_MISO  12
#define CAP1188_CLK  13
#define CAP1188_SENSITIVITY_REG 0x1F

#define NUM_AUDIO_FILES 13


// For I2C, connect SDA to your Arduino's SDA pin, SCL to SCL pin
// On UNO/Duemilanove/etc, SDA == Analog 4, SCL == Analog 5

// I2C, with reset pin
Adafruit_CAP1188 cap = Adafruit_CAP1188(CAP1188_RESET);
// sensitivity level, lower is more sensitive
const int CAP1188_SENSITIVITY_LVL = 0x47;
// track which audio file should be played
int audio_index;
// bit vector of which caps were touched last time
uint8_t prev_touched;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting LeafMeAlone");
  // Initialize the sensor, if using i2c you can pass in the i2c address
  //if (!cap.begin(0x28)) {
  if (!cap.begin()) {
    Serial.println("CAP1188 not found");
    while (1);
  }
  Serial.println("CAP1188 found!");
  cap.writeRegister(CAP1188_SENSITIVITY_REG, CAP1188_SENSITIVITY_LVL);

  // initialise the music player
  if (! musicPlayer.begin()) { 
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));
  
   if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }

  // list files
  printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20,20);

  audio_index = 0;
  prev_touched = 0;
}

void loop() {
  uint8_t touched = cap.touched();

  if (touched == 0) {
    // No touch detected
    prev_touched = touched;
    return;
  }
  
  for (uint8_t i=0; i<8; i++) {
    if (touched & (1 << i)) {
      Serial.print("C"); Serial.print(i+1); Serial.print("\t");
    }
  }

  //play audio file if something changed
  if (touched ^ prev_touched) {
      switch (audio_index) {
        case 0:
          Serial.println("0");
          musicPlayer.playFullFile("/track000.mp3");
          break;
        case 1:
          Serial.println("1");
          musicPlayer.playFullFile("/track00A.mp3");
          break;
        case 2:
          Serial.println("2");
          musicPlayer.playFullFile("/track00B.mp3");
          break;
        case 3:
          Serial.println("3");
          musicPlayer.playFullFile("/track00C.mp3");
          break;
        case 4:
          Serial.println("4");
          musicPlayer.playFullFile("/track001.mp3");
          break;
        case 5:
          Serial.println("5");
          musicPlayer.playFullFile("/track002.mp3");
          break;
        case 6:
          Serial.println("6");
          musicPlayer.playFullFile("/track003.mp3");
          break;
        case 7:
          Serial.println("7");
          musicPlayer.playFullFile("/track004.mp3");
          break;
        case 8:
          Serial.println("8");
          musicPlayer.playFullFile("/track005.mp3");
          break;
        case 9:
          Serial.println("9");
          musicPlayer.playFullFile("/track006.mp3");
          break;
        case 10:
          Serial.println("10");
          musicPlayer.playFullFile("/track007.mp3");
          break;
        case 11:
          Serial.println("11");
          musicPlayer.playFullFile("/track008.mp3");
          break;
        case 12:
          Serial.println("12");
          musicPlayer.playFullFile("/track009.mp3");
          break;
        default:
          Serial.println("default");
          break;
      }

      audio_index += 1;
      audio_index = audio_index % NUM_AUDIO_FILES;
    }
  Serial.println();
  delay(500);

  prev_touched = touched;
}

/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
