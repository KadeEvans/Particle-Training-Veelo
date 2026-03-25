#include "Particle.h"

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

constexpr pin_t LED_PIN = D7;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Log.info("Boot");
}

void loop() {
  static uint32_t last = 0;
  if (millis() - last >= 1000) {
    last = millis();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    Log.info("Heartbeat (ms=%lu)", millis());
  }
}

