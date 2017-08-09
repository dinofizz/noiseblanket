/*
  2017 Dino Fizzotti

  NoiseBlanket pairs the Adafruit WaveShield with an IR receiver to create a
  remote control capable white noise player.

  This sketch is essentially a modification of the existing Adafruit example
  sketch for the WaveShield "daphc.pde":

  https://github.com/adafruit/WaveHC/blob/master/WaveHC/examples/daphc/daphc.pde
  
  (There doesn't appear to be any license info in that repo, but I am assuming
  that as it is included as a sample sketch that it is OK to tweak and
  distribute with attribution)
*/

#include <IRremote.h>
#include <WaveHC.h>
#include <WaveUtil.h>

// [Dino] Variables and definitions for the IR receiver.
#define RECV_PIN 7
IRrecv irrecv(RECV_PIN);
decode_results results;

// [Dino] IR codes for "PC Remote"
#define VOLUME_UP   3223725573
#define VOLUME_DOWN 694752261
#define POWER       3243761281
#define PLAY_PAUSE  1987354549
#define STOP        2262137829
#define MUTE        3155862785

// [Dino] Variables required for operation of the Adafruit WaveShield
SdReader card;
FatVolume vol;
FatReader root;
WaveHC wave;

uint8_t dirLevel;
dir_t dirBuf;
uint8_t volumeLevel;
uint8_t storedVolume;


// [Dino] Miscellaneous defines
// Unintuitive, but the larger the value the lower the volume!
#define MAX_VOLUME 0
#define MIN_VOLUME 12
#define INITIAL_VOLUME 3
#define ACTIVITY_INTERVAL_MILLIS 10000

#define LED_PIN 6

unsigned long savedMillis;

/*
   Define macro to put error messages in flash memory
*/
#define error(msg) error_P(PSTR(msg))

void play(FatReader &dir);

/*
  Setup - runs before anything else.
*/
void setup() {
  Serial.begin(9600);

  putstring_nl("\nWave test!");

  putstring("Free RAM: ");
  Serial.println(FreeRam());

  if (!card.init()) {
    error("Card init. failed!");
  }

  card.partialBlockRead(true);

  uint8_t part;
  for (part = 0; part < 5; part++) {
    if (vol.init(card, part))
      break;
  }
  if (part == 5) {
    error("No valid FAT partition!");
  }

  putstring("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(), DEC);

  if (!root.openRoot(vol)) {
    error("Can't open root dir!");
  }

  putstring_nl("Files found (* = fragmented):");

  root.ls(LS_R | LS_FLAG_FRAGMENTED);

  irrecv.enableIRIn();

  volumeLevel = INITIAL_VOLUME;

  pinMode(LED_PIN, OUTPUT);

  blink(2, 200); // [Dino] Everything OK at this point.
}


/*
  Main loop.
*/
void loop() {
  root.rewind();
  play(root);
}


/*
   print error message and halt
*/
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  errorLoop();
}


/*
   print error message and halt if SD I/O error, great for debugging!
*/
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  errorLoop();
}


/*
  Helper function which blinks the LED the specified number of times for the
  specified duration.
*/
void blink(uint8_t repetitions, uint16_t delay_duration) {
  for (int i = 0; i < repetitions; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(delay_duration);
    digitalWrite(LED_PIN, LOW);
    delay(delay_duration);
  }
}

/*
  Helper function which blinks the LED at a rapid pace, endlessly.
*/
void errorLoop() {
   while (1) {
    blink(10, 100); // [Dino] Blink rapidly to indicate something went wrong.
  }
}

void activityBlink() {
  unsigned long currentMillis = millis();
  unsigned long deltaMillis = currentMillis - savedMillis;

  if (deltaMillis > ACTIVITY_INTERVAL_MILLIS) {
    blink(1, 200);
    savedMillis = currentMillis;
  }
}

/*
   play recursively - possible stack overflow if subdirectories too nested
*/
void play(FatReader &dir) {
  FatReader file;
  while (dir.readDir(dirBuf) > 0) {

    if (!DIR_IS_SUBDIR(dirBuf)
        && strncmp_P((char *)&dirBuf.name[8], PSTR("WAV"), 3)) {
      continue;
    }

    Serial.println();

    for (uint8_t i = 0; i < dirLevel; i++) {
      Serial.write(' ');
    }

    if (!file.open(vol, dirBuf)) {
      error("file.open failed");
      blink(10, 100); // [Dino] Blink rapidly to indicate something went wrong.
    }

    if (file.isDir()) {
      putstring("Subdir: ");
      printEntryName(dirBuf);
      Serial.println();
      dirLevel += 2;

      play(file);
      dirLevel -= 2;
    }
    else {
      putstring("Playing ");
      printEntryName(dirBuf);
      if (!wave.create(file)) {
        putstring(" Not a valid WAV");
        blink(10, 100); // [Dino] Blink rapidly to indicate something went wrong.
      } else {
        Serial.println();

        wave.volume = volumeLevel;
        wave.play();

        savedMillis = millis();

        while (wave.isplaying) {

          activityBlink();

          if (irrecv.decode(&results)) {
            unsigned long ir_result = results.value;            

            switch (ir_result) {
              case VOLUME_UP:
                if (volumeLevel > MAX_VOLUME && volumeLevel != MIN_VOLUME) {
                  wave.volume--;
                  volumeLevel--;
                }
                break;
              case VOLUME_DOWN:
                if (volumeLevel < MIN_VOLUME && volumeLevel != MIN_VOLUME) {
                  wave.volume++;
                  volumeLevel++;
                }
                break;
              case POWER:
                if (volumeLevel != MIN_VOLUME) {
                  storedVolume = volumeLevel;
                  wave.volume = MIN_VOLUME;
                  volumeLevel = MIN_VOLUME;
                }
                else {
                  wave.volume = storedVolume;
                  volumeLevel = storedVolume;
                }
                break;
              default:
                break;
            }
            
            blink(1, 200); // [Dino] Blink to indicate we did receive something.
            irrecv.resume();
          }

          delay(100);
        }
        
        sdErrorCheck();
      }
    }
  }
}
