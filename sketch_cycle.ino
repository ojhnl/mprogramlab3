// Цикл мигания по кнопке: 5 с мигает (1 Гц), 5 с выключен, и так по кругу.
// Три прерывания: Timer1 - переключает светодиод каждые 0,5 с,
// Timer2 - отсчитывает 5 с и меняет фазу, кнопка (пин 2 -> GND) - перезапускает цикл.

const int PIN_LED = 13, PIN_BUTTON = 2;

volatile bool running = false;      // цикл запущен кнопкой
volatile bool blinkPhase = true;    // true - фаза мигания, false - пауза
volatile bool pressed = false;      // флаг: было нажатие (для вывода в консоль)
volatile unsigned int ticks = 0;    // счётчик тиков Timer2 (1 тик = 4 мс)
unsigned long lastPress = 0;        // время последнего принятого нажатия (антидребезг)
unsigned long prevPress = 0;        // время предыдущего нажатия (для вывода в loop)

// Timer1: раз в 0,5 с переключает светодиод, только в фазе мигания
ISR(TIMER1_COMPA_vect) {
  if (running && blinkPhase) digitalWrite(PIN_LED, !digitalRead(PIN_LED));
}

// Timer2: каждые 4 мс; 1250 тиков = 5 с -> смена фазы
ISR(TIMER2_COMPA_vect) {
  if (running && ++ticks >= 1250) {
    ticks = 0;
    blinkPhase = !blinkPhase;
    digitalWrite(PIN_LED, LOW);          // при смене фазы светодиод выключен, дальше мигает Timer1
  }
}

// Кнопка: остановить всё и начать цикл с мигания
void onButton() {
  if (millis() - lastPress >= 50) {      // антидребезг 50 мс
    lastPress = millis();
    running = true;
    blinkPhase = true;
    ticks = 0;
    TCNT1 = 0;
    TCNT2 = 0;
    digitalWrite(PIN_LED, HIGH);
    pressed = true;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), onButton, FALLING);

  cli();                                 // отключить глобальные прерывания
  // Timer1: CTC, делитель 1024, 16 МГц / 1024 = 15625 Гц; 0,5 с = 7812 отсчётов
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
  OCR1A = 7811;
  TIMSK1 = (1 << OCIE1A);
  // Timer2: CTC, делитель 256, 16 МГц / 256 = 62500 Гц; 250 отсчётов = 4 мс
  TCCR2A = (1 << WGM21);
  TCCR2B = (1 << CS22) | (1 << CS21);
  OCR2A = 249;
  TIMSK2 = (1 << OCIE2A);
  sei();                                 // включить глобальные прерывания
}

void loop() {
  if (pressed) {                         // печать - в loop, не в обработчике
    pressed = false;
    unsigned long now = millis();
    Serial.print("Time since last press, ms: ");
    Serial.println(now - prevPress);     // первое нажатие: время с запуска платы
    prevPress = now;
  }
}