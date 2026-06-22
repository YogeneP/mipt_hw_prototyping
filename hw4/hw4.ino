#include <SPI.h>

#define TACT 167
#define DOT_TACTS 3
#define DIGIT_TACTS 6
#define LATCH_PIN 9

uint8_t seg_digits[10] = {
    0b01111110, // 0
    0b00001100, // 1
    0b10110110, // 2
    0b10011110, // 3
    0b11001100, // 4
    0b11011010, // 5
    0b11111010, // 6
    0b00001110, // 7
    0b11111110, // 8
    0b11011110  // 9
};

uint64_t prev_tick = 0;
uint64_t tick = 0;
uint8_t dot_tact = 0;
uint8_t digit_tact = 0;
uint8_t leds = 1;
uint8_t digit = 0;
bool dot = 0;

void setup() {
  pinMode(LATCH_PIN,OUTPUT);
  Serial.begin(115200);
  SPI.begin();
  digitalWrite(LATCH_PIN,LOW);
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  digitalWrite(LATCH_PIN,HIGH);
}

void loop() {
  tick = millis();
  if (dot_tact >= DOT_TACTS) {
    dot = !dot;
    dot_tact = 0;
  }
  if (digit_tact >= DIGIT_TACTS) {
    if(++digit > 9) digit = 0;
    digit_tact = 0;
  }
  if (tick - prev_tick > TACT) {
    if(leds > 63) leds = 1; 
    leds = (leds << 1 | 0x01);
    digitalWrite(LATCH_PIN,LOW);
    SPI.transfer(leds);
    SPI.transfer(seg_digits[digit] | (dot & 0x01));
    digitalWrite(LATCH_PIN,HIGH);
    prev_tick = tick;
    dot_tact++;
    digit_tact++;
  } 
}
