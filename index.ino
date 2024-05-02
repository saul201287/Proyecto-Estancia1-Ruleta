int buttonPin = 6;
int ledPins[] = {7, 8, 9, 10};
int numLEDs = 4;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  for (int i = 0; i < numLEDs; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    delay(100); // Debounce
    while (digitalRead(buttonPin) == LOW) {}
    delay(100); // Espera adicional para evitar múltiples detecciones
    executeEffect();
  }
}

void executeEffect() {
  int delaytime = 200;
  int stopRepeats = 3;
  int stopLED = -1;
  for (int repeat = 0; repeat < stopRepeats; repeat++) {
    if (repeat == 2) {
      delaytime = 400;
    }

    for (int i = 0; i < numLEDs; i++) {

      digitalWrite(ledPins[i], HIGH);

      delay(delaytime);

      digitalWrite(ledPins[i], LOW);

      if (repeat == stopRepeats - 1 && i == stopLED) {
        digitalWrite(ledPins[stopLED], HIGH);
        return;
      }
    }
    stopLED = random(0, numLEDs);
  }
}
