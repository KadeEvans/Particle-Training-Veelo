#include "Particle.h"

// Baseline firmware (main branch)
// This branch is intentionally simple: it blinks the built-in LED and prints logs.
// The training examples live in the `training/*` branches.

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

constexpr pin_t LED_PIN = D7;

void setup() {
  // Set the LED pin as an output so we can blink it.
  pinMode(LED_PIN, OUTPUT);
  Log.info("Boot: baseline");
}

void loop() {
  // Blink the LED once per second and log a heartbeat message.
  static uint32_t last = 0;
  if (millis() - last >= 1000) {
    last = millis();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    Log.info("Heartbeat (ms=%lu)", millis());
  }
}

