int delay_time = 1000;

void blink(int value, int delay_time);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  blink(1, delay_time);
  blink(0, delay_time);
}

void blink(int value, int delay_time) {
    digitalWrite(LED_BUILTIN, value);
    delay(delay_time);
}