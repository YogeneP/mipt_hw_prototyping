#define LED_TICKS 200
#define BTN_TICKS 10


void setup() {
  pinMode(13,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(2,INPUT_PULLUP);
}

boolean led_state = false;
boolean trig_led_state = false;
boolean btn_state = false;

long long int tick_led = 0;
long long int tick_btn = 0;

void loop() {
  if (millis() - tick_led > LED_TICKS) {
    tick_led = millis();
    led_state = !led_state;
    digitalWrite(13, led_state);
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
