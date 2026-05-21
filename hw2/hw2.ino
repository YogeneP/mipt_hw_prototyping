#define LED_TICKS 100
#define BTN_TICKS 10


void setup() {
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(2,INPUT_PULLUP);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(7,OUTPUT);
}

boolean led_state = false;
boolean trig_led_state = false;
boolean btn_state = false;

long long int tick_led = 0;
long long int tick_btn = 0;
int var = 0;
int led_i = 0;
int leds[] = {12, 7, 11, 10, 9, 8};
int buf[32] = {0};

int filter(int var, int* buf);

void loop() {
  if (millis() - tick_led > LED_TICKS) {
    tick_led = millis();
    led_state = !led_state;
    digitalWrite(13, led_state);
    Serial.print("Pot: ");
    Serial.println(analogRead(A0));
    Serial.print("Light: ");
    var = analogRead(A1);
    Serial.println(var);
    Serial.print("Filtered light: ");
    Serial.println(filter(var,buf));
    
    led_i = map(var, 50, 400, 0, 6); 
    for (int i = 1; i <= 6; i++) {
      if (i < led_i) { 
        digitalWrite(leds[i-1], LOW);
      } else {
        digitalWrite(leds[i-1], HIGH);
      }
    }
  }
  
  if (millis() - tick_btn > BTN_TICKS) {
    tick_btn = millis();
    if (!digitalRead(2)) {
      if (!btn_state) {
        btn_state = true;
        trig_led_state = !trig_led_state;
        digitalWrite(3,trig_led_state);   
      }
    } else {
      btn_state = false;      
    }
  }
}

int filter(int val, int* buf) {
  static int buf_i = 0;
  static boolean buf_full = false;
  static long int sum = 0;
  int filtered = 0;

  sum = sum - buf[buf_i] + val;
  buf[buf_i] = val;
  buf_i++;
  filtered = buf_full ? sum >> 5 : sum / buf_i;
  if(buf_i > 31) {
    buf_full = true;
    buf_i = 0;
  }
    
  return filtered;
}
