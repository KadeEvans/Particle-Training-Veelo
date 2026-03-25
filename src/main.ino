#include "Particle.h"

// =========================
// Branch: training/cloud-functions
// Concept: Cloud Functions (Particle.function)
//
// What you learn here:
// - How to expose a function your device can run from the Particle Cloud
// - How to parse a simple String argument ("on", "off", "toggle", or a number)
// - How to return a status code back to the caller
//
// How to verify:
// - In the Particle Console, open your device, then use the "Functions" section
// - Call:
//   - led("on"), led("off"), led("toggle")
//   - rate("250")  (sets blink period to 250ms)
// =========================

// Device OS 6.2.0+ enables system threading by default, so we do not need
// SYSTEM_THREAD(ENABLED) anymore (avoids a warning during compile).

SerialLogHandler logHandler(LOG_LEVEL_INFO);

constexpr pin_t LED_PIN = D7;

// These are "global" so both loop() and the cloud functions can read/write them.
// (volatile isn't strictly required here, but it communicates: "this may change outside
// of the current code path", which can help beginners reading the code.)
volatile bool ledState = false;
volatile uint32_t blinkPeriodMs = 1000;

// Cloud Function #1: "led"
// The Particle Cloud calls this function and passes ONE String argument.
// You return an int:
// - positive/zero values: usually "success" (you choose what they mean)
// - negative values: usually "error" (common convention)
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

  // Anything else: we didn't understand the command.
  return -1;
}

// Cloud Function #2: "rate"
// Sets how fast the LED blinks (milliseconds between toggles).
int rateFn(String arg) {
  arg.trim();

  // toInt() parses the leading number in the string.
  // Examples: "250" -> 250, "250ms" -> 250, "abc" -> 0
  const int ms = arg.toInt();

  // Basic validation: don't allow super-fast or super-slow values.
  if (ms < 100 || ms > 60000) {
    return -1;
  }
  blinkPeriodMs = (uint32_t)ms;
  return (int)blinkPeriodMs;
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Log.info("Boot: cloud functions example");

  // Register cloud functions.
  // The first parameter is the function name you will see in the Console/API.
  // The second parameter is the C++ function to run on the device.
  Particle.function("led", ledFn);
  Particle.function("rate", rateFn);
  Log.info("Cloud functions registered: led, rate");
}

void loop() {
  // Apply the current LED state to the physical pin.
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);

  // Simple blink logic so you can see the board doing something even without cloud calls.
  static uint32_t lastBlink = 0;
  if (millis() - lastBlink >= blinkPeriodMs) {
    lastBlink = millis();
    ledState = !ledState;
    Log.info("Blink (period=%lu ms, ms=%lu)", blinkPeriodMs, millis());
  }
}
