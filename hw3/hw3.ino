#include <SPI.h>
#include <Adafruit_GFX.h>
#include "ST7789_AVR.h"

#define TFT_DC 10
#define TFT_RST 9 
#define TFT_BLK 3
#define TFT_CS -1
#define ST7789_VSCRDEF 0x33
#define ST7789_VSCSAD 0x37

#define LIGHT_PIN A0
#define POT_PIN A5

#define SCR_WD 170
#define SCR_HT 320

#define GRAD_HEIGHT 100
#define SCROLL_TFA 15
#define SCROLL_BFA 0

#define BLK_TICK 10

#define MAX_BLK 180

#define DC_DATA    *dcPort |= dcMask
#define DC_COMMAND *dcPort &= ~dcMask
#define CS_IDLE    *csPort |= csMask
#define CS_ACTIVE  *csPort &= ~csMask
#define DARK_BLUE 0x0010

ST7789_AVR tft = ST7789_AVR(TFT_DC, TFT_RST, TFT_CS);

unsigned long long int blk_ms = 0;
unsigned long long int curr_ms = 0;
int curr_pot = 0;
int prev_pot = 0;
int curr_light = 0;
int prev_light = 0;
int8_t csPin = TFT_CS;
int8_t dcPin = TFT_DC;
int8_t rstPin = TFT_RST;
uint8_t  csMask, dcMask;
volatile uint8_t  *csPort, *dcPort;

void setup() {
  dcPort = portOutputRegister(digitalPinToPort(dcPin));
  dcMask = digitalPinToBitMask(dcPin);
  csPort = portOutputRegister(digitalPinToPort(csPin));
  csMask = digitalPinToBitMask(csPin);
  Serial.begin(9600);
  tft.init(SCR_WD, SCR_HT);
  pinMode(TFT_BLK,OUTPUT);
  tft.fillScreen(CYAN);
  tft.setAddrWindow(0, 0, SCR_WD - 1, SCROLL_TFA);
  writeMulti(DARK_BLUE, SCR_WD * SCROLL_TFA+1);
  tft.drawRect(0,0,SCR_WD,13,WHITE);
  analogWrite(TFT_BLK, MAX_BLK);
  curr_pot = analogRead(POT_PIN);
  curr_light = analogRead(LIGHT_PIN);
  Serial.print("BLK pin: ");
  Serial.println(curr_pot);
  drawVertGrad(SCROLL_TFA, GRAD_HEIGHT, DARK_BLUE, CYAN);
  tft.setScrollArea(SCROLL_TFA, SCROLL_BFA);
}

uint16_t scroll_offset = 0;
int16_t delta_scroll_offset = 0;
int8_t scroll_offset_inc = 0;

void loop() {
  curr_ms = millis();
  if (curr_ms - blk_ms > BLK_TICK) {
    blk_ms = curr_ms;
    curr_pot = analogRead(POT_PIN);
    curr_light = analogRead(LIGHT_PIN);
//    Serial.print("LIGHT=");
//    Serial.println(curr_light);

    if(abs(prev_pot - curr_pot) > 16) {
      analogWrite(TFT_BLK,map(curr_pot,0,1023,0,MAX_BLK));
      if (curr_pot > prev_pot) {
        tft.setAddrWindow(2, 2, map(curr_pot,0,1023,2,SCR_WD - 3), 10);
        writeMulti(WHITE, SCR_WD*9);
      } else {
        tft.setAddrWindow(map(curr_pot,0,1023,2,SCR_WD - 2), 2, SCR_WD - 2, 10);
        writeMulti(DARK_BLUE, SCR_WD*9);
      }
      prev_pot = curr_pot;
    }          
    if(abs(prev_light - curr_light) > 6) {
      if (curr_light < 50) delta_scroll_offset = SCR_HT - SCROLL_BFA - SCROLL_TFA - GRAD_HEIGHT;
      else if (curr_light > 750) delta_scroll_offset = 0;
      else delta_scroll_offset = map(curr_light,750,50,0,SCR_HT - SCROLL_BFA - SCROLL_TFA - GRAD_HEIGHT);
      delta_scroll_offset -= scroll_offset;
      prev_light = curr_light;
    }
    if (delta_scroll_offset) {
      if(delta_scroll_offset > 0) {
        scroll_offset++;
        delta_scroll_offset--;
        tft.setAddrWindow(0, SCR_HT - SCROLL_BFA - scroll_offset + 1, SCR_WD - 1, SCR_HT - SCROLL_BFA - scroll_offset + 1);
        writeMulti(DARK_BLUE, SCR_WD);
      } else {
        scroll_offset--;
        delta_scroll_offset++;
        tft.setAddrWindow(0, SCR_HT - SCROLL_BFA - scroll_offset - 1, SCR_WD - 1, SCR_HT - SCROLL_BFA - scroll_offset - 1);
        writeMulti(CYAN, SCR_WD);
      }
      tft.setScroll(SCR_HT - SCROLL_BFA - scroll_offset);
    }
  }
}

//6...7 bit shift to increase color step accuracy
void drawVertGrad(unsigned int v_pos, unsigned int height, unsigned int top_color, unsigned int bottom_color) {
  byte top_color_r = (top_color >> 11) & 0x1F;
  byte top_color_g = (top_color >> 5) & 0x3F;
  byte top_color_b = top_color & 0x1F;
  Serial.print("top color: ");
  Serial.print("r=");
  Serial.print(top_color_r);
  Serial.print("; g=");
  Serial.print(top_color_g);
  Serial.print("; b=");
  Serial.println(top_color_b);

  byte bottom_color_r = (bottom_color >> 11) & 0x1F;
  byte bottom_color_g = (bottom_color >> 5) & 0x3F;
  byte bottom_color_b = bottom_color & 0x1F;
  Serial.print("bottom color: ");
  Serial.print("r=");
  Serial.print(bottom_color_r);
  Serial.print("; g=");
  Serial.print(bottom_color_g);
  Serial.print("; b=");
  Serial.println(bottom_color_b);

  int16_t r_step = ((((int16_t)bottom_color_r - (int16_t)top_color_r)) << 7) / ((int16_t)height-1);
  int16_t g_step = ((((int16_t)bottom_color_g - (int16_t)top_color_g)) << 7) / ((int16_t)height-1);
  int16_t b_step = ((((int16_t)bottom_color_b - (int16_t)top_color_b)) << 7) / ((int16_t)height-1);
  Serial.print("color step: ");
  Serial.print("r=");
  Serial.print(r_step);
  Serial.print("; g=");
  Serial.print(g_step);
  Serial.print("; b=");
  Serial.println(b_step);

  uint16_t curr_color = top_color;
  uint16_t curr_color_r = top_color_r << 7;
  uint16_t curr_color_g = top_color_g << 7;
  uint16_t curr_color_b = top_color_b << 7;
  
  int y = v_pos;
  uint16_t x = 0;

  tft.setAddrWindow(0, v_pos, SCR_WD-1, (v_pos + height >= SCR_HT) ? SCR_HT - 1 : v_pos + height);
  
  while ((y < v_pos + height) && (y < SCR_HT)) {
    if (y >= 0) {
      curr_color_r = curr_color_r + r_step; 
      curr_color_g = curr_color_g + g_step; 
      curr_color_b = curr_color_b + b_step;
      curr_color = ((curr_color_r << 4) & 0xF800) | ((curr_color_g >> 2) & 0x07E0) | ((curr_color_b >> 7) & 0x001F);   
      writeMulti(curr_color, SCR_WD);
      y++;
    }
  }
  CS_IDLE; 
}

// ----------------------------------------------------------
// speed test results:
// in AVR best performance mode -> about 6.9 Mbps
// in compatibility mode (SPI.transfer(c)) -> about 4 Mbps
inline void writeSPI(uint8_t c) 
{
    SPDR = c;
    asm volatile("rjmp .+0\n");
    asm volatile("rjmp .+0\n");
    asm volatile("rjmp .+0\n");
    asm volatile("rjmp .+0\n");
}

// ----------------------------------------------------------
// fast method to send multiple 16-bit values via SPI
inline void writeMulti(uint16_t color, uint16_t num) {
  asm volatile
  (
  "next1:\n"
    "out %[spdr],%[hi]\n"
    "rjmp .+0\n"  // wait 8*2+1 = 17 cycles
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "nop\n"
    "out %[spdr],%[lo]\n"
    "rjmp .+0\n"  // wait 6*2+1 = 13 cycles + sbiw + brne
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "nop\n"
    "sbiw %[num],1\n"
    "brne next1\n"
    : [num] "+w" (num)
    : [spdr] "I" (_SFR_IO_ADDR(SPDR)), [lo] "r" ((uint8_t)color), [hi] "r" ((uint8_t)(color>>8))
  );
} 
// ----------------------------------------------------------
// fast method to send multiple 16-bit values from RAM via SPI
inline void ST7789_AVR::copyMulti(uint8_t *img, uint16_t num)
{
  uint8_t lo,hi;
  asm volatile
  (
  "nextCopy:\n"
    "ld  %[hi],%a[img]+\n"
    "ld  %[lo],%a[img]+\n"
    "out %[spdr],%[lo]\n"
    "rjmp .+0\n"  // wait 8*2+1 = 17 cycles
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "nop\n"
    "out %[spdr],%[hi]\n"
    "rjmp .+0\n"  // wait 4*2+1 = 9 cycles + sbiw + brne + ld*2
    "rjmp .+0\n"
    "rjmp .+0\n"
    "rjmp .+0\n"
    "nop\n"
    "sbiw %[num],1\n"
    "brne nextCopy\n"
    : [num] "+w" (num)
    : [spdr] "I" (_SFR_IO_ADDR(SPDR)), [img] "e" (img), [lo] "r" (lo), [hi] "r" (hi)
  );
} 
