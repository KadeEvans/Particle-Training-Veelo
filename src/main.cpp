#include "Particle.h"

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

constexpr pin_t LED_PIN = D7;
constexpr uint32_t PUBLISH_PERIOD_MS = 10 * 1000;

const char *EVENT_NAME = "training/cloud_publish";

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Log.info("Boot");
}

void loop() {
  static uint32_t lastBlink = 0;
  if (millis() - lastBlink >= 1000) {
    lastBlink = millis();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }

  static uint32_t lastPublish = 0;
  if (millis() - lastPublish >= PUBLISH_PERIOD_MS) {
    lastPublish = millis();

    // Keep payload small; publish is rate-limited and size-limited.
    char data[64];
    snprintf(data, sizeof(data), "{\"ms\":%lu}", millis());

    const bool ok = Particle.publish(EVENT_NAME, data, PRIVATE);
    Log.info("Particle.publish(%s) -> %s, data=%s", EVENT_NAME, ok ? "ok" : "fail", data);
  }
}

