#include "Particle.h"

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

constexpr pin_t LED_PIN = D7;

volatile bool ledState = false;
volatile uint32_t blinkPeriodMs = 1000;

int ledFn(String arg) {
  arg.trim();
  arg.toLowerCase();

  if (arg == "on" || arg == "1" || arg == "true") {
    ledState = true;
    return 1;
  }
  if (arg == "off" || arg == "0" || arg == "false") {
    ledState = false;
    return 0;
  }
  if (arg == "toggle") {
    ledState = !ledState;
    return ledState ? 1 : 0;
  }
  return -1;
}

int rateFn(String arg) {
  arg.trim();
  const int ms = arg.toInt();
  if (ms < 100 || ms > 60000) {
    return -1;
  }
  blinkPeriodMs = (uint32_t)ms;
  return (int)blinkPeriodMs;
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Log.info("Boot");

  // Cloud Functions: call from the Particle Console or API.
  // - led: "on" | "off" | "toggle"
  // - rate: blink period in ms (100..60000)
  Particle.function("led", ledFn);
  Particle.function("rate", rateFn);
  Log.info("Cloud functions registered: led, rate");
}

void loop() {
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);

  static uint32_t lastBlink = 0;
  if (millis() - lastBlink >= blinkPeriodMs) {
    lastBlink = millis();
    ledState = !ledState;
    Log.info("Blink (period=%lu ms, ms=%lu)", blinkPeriodMs, millis());
  }
}

