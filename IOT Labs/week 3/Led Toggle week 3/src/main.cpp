#include <Arduino.h>

#define LED_PIN     5      // On-board LED
#define BUTTON_PIN  34     // Button to GND
#define DEBOUNCE_MS 50

hw_timer_t *My_timer = NULL;
volatile bool ledEnabled = true;
volatile uint32_t lastButtonMs = 0;

// Timer ISR: blink LED only if enabled
void IRAM_ATTR onTimer() {
  if (ledEnabled) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
}

// Button ISR: toggle enable flag (with debounce)
void IRAM_ATTR onButton() {
  uint32_t now = millis();
  if (now - lastButtonMs > DEBOUNCE_MS) {
    lastButtonMs = now;
    ledEnabled = !ledEnabled;   // toggle flag
    if (!ledEnabled) {
      digitalWrite(LED_PIN, LOW); // reset LED when disabled
    }
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Button interrupt (FALLING edge = pressed)
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButton, FALLING);

  // Create timer: timer 0, prescaler 80 → 1 tick = 1 µs
  My_timer = timerBegin(0, 80, true);

  // Attach interrupt
  timerAttachInterrupt(My_timer, &onTimer, true);

  // 0.5 second = 500,000 microseconds
  timerAlarmWrite(My_timer, 500000, true);
  timerAlarmEnable(My_timer);
}

void loop() {
  // nothing needed — interrupts handle everything
}
