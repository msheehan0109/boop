#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <Servo.h>
Servo boop;
#define BREAKOUT_RESET  9
#define BREAKOUT_CS     10
#define BREAKOUT_DCS    8
#define SHIELD_RESET  -1
#define SHIELD_CS     7
#define SHIELD_DCS    6
#define VS1053_RX  2
#define CARDCS 4
#define DREQ 3
int pos = 0;

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

int x;
void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Simple Test");
  musicPlayer.setVolume(10, 10);

  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  boop.attach(7);

}
void loop() {
  x = digitalRead(5);
  if (x == HIGH)
  {
    boop.write(140);
    delay(100);
    boop.write(190);
    delay(100);
  }

}
