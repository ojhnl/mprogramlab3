const int cols[4] = {4, 5, 6, 7};
const int rows[4] = {8, 9, 10, 11};
const char keys[4][4] = {{'1', '2', '3', 'a'}, {'4', '5', '6', 'b'}, {'7', '8', '9', 'c'}, {'*', '0', '#', 'd'}};

volatile bool pressed = false;
volatile unsigned long last = 0;

ISR(PCINT0_vect) {
  if (millis() - last >= 50) {
    last = millis();
    pressed = true;
  }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    digitalWrite(cols[i], LOW);
    pinMode(cols[i], OUTPUT);
    pinMode(rows[i], INPUT_PULLUP);
  }
  PCICR |= (1 << PCIE0);
  PCMSK0 |= 0x0F;
}

void loop() {
  if (!pressed) return;
  delay(20);
  PCICR &= ~(1 << PCIE0);
  for (int c = 0; c < 4; c++) {
    for (int i = 0; i < 4; i++) pinMode(cols[i], i == c ? OUTPUT : INPUT);
    for (int r = 0; r < 4; r++) {
      if (digitalRead(rows[r]) == LOW) {
        Serial.println(keys[3-c][3-r]);
        tone(12, 1000, 100);
        delay(50);
        while (digitalRead(rows[r]) == LOW);
        delay(50);
      }
    }
  }
  for (int i = 0; i < 4; i++) pinMode(cols[i], OUTPUT);
  pressed = false;
  PCIFR |= (1 << PCIF0);
  PCICR |= (1 << PCIE0);
}