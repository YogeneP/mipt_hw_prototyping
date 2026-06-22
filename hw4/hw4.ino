#include <SPI.h>

#define TACT 167
#define DOT_TACTS 3
#define DIGIT_TACTS 6
#define DOT_MASK 0x80
#define LATCH_PIN 9

typedef union {
  struct {
    uint8_t leds  :8;
    uint8_t digit :7;
    uint8_t dot   :1;
  } bits;
  uint16_t package;
} sample_t;
/*
const uint8_t seg_digits[10] = {
    0xFC, // 0 (was 0x3F)
    0x60, // 1 (was 0x06)
    0xDA, // 2 (was 0x5B)
    0xF2, // 3 (was 0x4F)
    0x66, // 4 (was 0x66)
    0xB6, // 5 (was 0x6D)
    0xBE, // 6 (was 0x7D)
    0xE0, // 7 (was 0x07)
    0xFE, // 8 (was 0x7F)
    0xF6  // 9 (was 0x6F)
};
*/

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
uint8_t digit = 0;
sample_t sample;

uint16_t deb16;
uint16_t deb8;

void setup() {
  pinMode(LATCH_PIN,OUTPUT);
  Serial.begin(115200);
  SPI.begin();
  digitalWrite(LATCH_PIN,LOW);
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  digitalWrite(LATCH_PIN,HIGH);
/*
  deb16 = 0b1000000000100000;
  Serial.println(deb16, BIN);
  SPI.transfer16(deb16);  
  digitalWrite(OE_PIN,LOW);
  delay(1000);
  digitalWrite(OE_PIN,HIGH);
  SPI.transfer16(0);

  delay(500);

  deb16 = 0b0000000000010000;
  Serial.println(deb16, BIN);
  SPI.transfer16(deb16);  
  digitalWrite(OE_PIN,LOW);
  delay(1000);
  digitalWrite(OE_PIN,HIGH);
  SPI.transfer16(0); 

  delay(500);  
  /* 0b10000000 DP
   * 0b01000000 A
   * 0b00100000 B
   * 0b00010000 C
   * 0b00001000 D 
   * 0b00000100 E
   * 0b00000010 F
   * 0b00000001 G
   * 
   */
/*  deb16 = 0b1000000100001000;
  Serial.println(deb16, BIN);
  SPI.transfer16(deb16);  
  digitalWrite(OE_PIN,LOW);
  delay(1000);
//  digitalWrite(OE_PIN,HIGH);
*/
}

void loop() {
  delay(1000);
  digitalWrite(LATCH_PIN,LOW);
  SPI.transfer(seg_digits[9-digit]);
  digitalWrite(LATCH_PIN,HIGH);
  delay(1000);
  digitalWrite(LATCH_PIN,LOW);
  SPI.transfer(seg_digits[digit]);
  // SPI.transfer(deb16 >> 8);
  digitalWrite(LATCH_PIN,HIGH);
  digit >= 9 ? digit = 0 : digit++;
/*  tick = millis();
  if (dot_tact >= DOT_TACTS) {
    sample.bits.dot = !sample.bits.dot;
    dot_tact = 0;
  }
  if (digit_tact >= DIGIT_TACTS) {
//        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    if(++digit > 9) digit = 0;
    sample.bits.digit = seg_digits[digit];
    digit_tact = 0;
  }
  if (tick - prev_tick > TACT) {
    if(sample.bits.leds >= 32) sample.bits.leds = 1; 
    else sample.bits.leds = (sample.bits.leds << 1);// | 0x01;
    sample.bits.digit = seg_digits[digit];
    Serial.println(sample.package, BIN);
    SPI.transfer16(sample.package);
    prev_tick = tick;
    dot_tact++;
    digit_tact++;
  } */
}
