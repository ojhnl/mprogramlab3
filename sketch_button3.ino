// Задание 1: ловим прерывание по нажатию кнопки (Arduino Uno).
// Кнопка на пине 2 (INT0). Нажатие считается в обработчике, печать - в loop().

const int PIN_BUTTON = 2;

volatile bool pressed = false;           // флаг: было нажатие (ставит обработчик)
volatile unsigned long lastPress = 0;    // время последнего принятого нажатия
unsigned long count = 0;                 // сколько нажатий поймали

// Обработчик прерывания: короткий, без delay и Serial, только ставит флаг
void onButton() {
  if (millis() - lastPress >= 50) {      // антидребезг 50 мс
    lastPress = millis();
    pressed = true;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUTTON, INPUT_PULLUP);     // кнопка между пином 2 и GND
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), onButton, FALLING);  // FALLING: HIGH -> LOW при нажатии
  // Для модуля с выводами GND/VCC/OUT, дающего HIGH при нажатии:
  //   pinMode(PIN_BUTTON, INPUT);  и режим RISING
}

void loop() {
  if (pressed) {
    pressed = false;
    count++;
    Serial.print("Interrupt! press #");
    Serial.println(count);
  }
}