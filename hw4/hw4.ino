#include <SPI.h>

#define TACT 167
#define DOT_TACTS 3
#define DIGIT_TACTS 6
#define DOT_MASK 0x80

typedef union {
  struct {
    uint8_t leds  :8;
    uint8_t digit :7;
    uint8_t dot   :1;
  } bits;
  uint16_t package;
} sample_t;

const uint8_t seg_digits[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

uint64_t prev_tick = 0;
uint64_t tick = 0;
uint8_t dot_tact = 0;
uint8_t digit_tact = 0;
uint8_t digit = 0;
sample_t sample;

void setup() {
  SPI.begin();
}

void loop() {
  tick += millis();
  if (dot_tact >= DOT_TACTS) {
    sample.bits.dot = !sample.bits.dot;
    dot_tact = 0;
  }
  if (digit_tact >= DIGIT_TACTS) {
    if(++digit > 9) digit = 0;
    sample.bits.digit = seg_digits[digit];
    digit_tact = 0;
  }
  if (tick - prev_tick > TACT) {
    if(sample.bits.leds > 63) sample.bits.leds = 0; 
    else sample.bits.leds = (sample.bits.leds << 1) | 0x01;
    SPI.beginTransaction(SPISettings(400000, LSBFIRST, SPI_MODE0));
    SPI.transfer(sample.package);
    prev_tick = tick;
    dot_tact++;
    digit_tact++;
  }
}
