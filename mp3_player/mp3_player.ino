#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <SoftwareSerial.h>
// http://arduino.cc/en/Reference/SPI
// http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf
// http://arduino.cc/en/Reference/attachInterrupt

#define BREAKOUT_RESET  9
#define BREAKOUT_CS     10
#define BREAKOUT_DCS    8

#define SHIELD_RESET  -1
#define SHIELD_CS     7
#define SHIELD_DCS    6
#define VS1053_RX  2

#define CARDCS 4

#define DREQ 3

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79


#define VS1053_GM1_OCARINA 41

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0

SoftwareSerial VS1053_MIDI(0, 2);
int x;
void setup() {
  x = digitalRead(5);
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Simple Test");
  VS1053_MIDI.begin(31250);

  // pinMode(VS1053_RESET, OUTPUT);
  // digitalWrite(VS1053_RESET, LOW);
  // delay(10);
  // digitalWrite(VS1053_RESET, HIGH);
  //delay(10);

  midiSetChannelBank(0, VS1053_BANK_DRUMS2);
  midiSetInstrument(0, VS1053_GM1_OCARINA);
  midiSetChannelVolume(0, 127);

  if (! musicPlayer.begin()) { // initialise the music player
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
  musicPlayer.setVolume(1, 1);

  // Timer interrupts are not suggested, better to use DREQ interrupt!
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

  /* if (x == HIGH)
    {
     Serial.println(F("Playing track 001"));
     musicPlayer.playFullFile("track001.mp3");
     // Play another file in the background, REQUIRES interrupts!
     Serial.println(F("Playing track 002"));
     musicPlayer.startPlayingFile("track002.mp3");
     Serial.println(F("Playing track 003"));
     musicPlayer.startPlayingFile("track003.mp3");
    }
  */
}

void loop() {
  x = digitalRead(5);
  if (x == HIGH)
  {
    Serial.println(F("Playing track 001"));
    musicPlayer.playFullFile("track001.mp3");
    // Play another file in the background, REQUIRES interrupts!
    Serial.println(F("Playing track 002"));
    musicPlayer.startPlayingFile("track002.mp3");
    Serial.println(F("Playing track 003"));
    musicPlayer.startPlayingFile("track003.mp3");
  }
  // File is playing in the background
  if (musicPlayer.stopped()) {
    Serial.println("Done playing music");
    while (1) {
      delay(10);  // we're done! do nothing...
    }
  }


  if (Serial.available()) {
    char c = Serial.read();

    // if we get an 's' on the serial console, stop!
    if (c == 's') {
      musicPlayer.stopPlaying();
    }

    // if we get an 'p' on the serial console, pause/unpause!
    if (c == 'p') {
      if (! musicPlayer.paused()) {
        Serial.println("Paused");
        musicPlayer.pausePlaying(true);
      } else {
        Serial.println("Resumed");
        musicPlayer.pausePlaying(false);
      }
    }
  }

  delay(100);

  for (uint8_t i = 27; i < 87; i++) {
    midiNoteOn(0, 72, 127);
    delay(100);
    midiNoteOff(0, 72, 127);
  }
}


/// File listing helper
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      //Serial.println("**nomorefiles**");
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void midiSetInstrument(uint8_t chan, uint8_t inst) {
  if (chan > 15) return;
  inst --; // page 32 has instruments starting with 1 not 0 :(
  if (inst > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_PROGRAM | chan);
  VS1053_MIDI.write(inst);
}

void midiSetChannelVolume(uint8_t chan, uint8_t vol) {
  if (chan > 15) return;
  if (vol > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(MIDI_CHAN_VOLUME);
  VS1053_MIDI.write(vol);
}

void midiSetChannelBank(uint8_t chan, uint8_t bank) {
  if (chan > 15) return;
  if (bank > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write((uint8_t)MIDI_CHAN_BANK);
  VS1053_MIDI.write(bank);
}

void midiNoteOn(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;

  VS1053_MIDI.write(MIDI_NOTE_ON | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void midiNoteOff(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;

  VS1053_MIDI.write(MIDI_NOTE_OFF | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

