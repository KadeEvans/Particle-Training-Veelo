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

SYSTEM_THREAD(ENABLED);

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

#include "Particle.h"

// =========================
// Branch: training/cloud-publish
// Concept: Cloud Publish (Particle.publish)
//
// What you learn here:
// - How to publish an event from a device to the Particle Cloud
// - How to include a small payload (data) with your event
// - How to avoid publishing too often (rate limits)
//
// How to verify:
// - Open the Particle Console and watch the event stream
// - Filter for the event name below: training/cloud_publish
// =========================

// Let the device application run in a separate thread from the cloud connection.
// This keeps loop() running even if the cloud connection is still coming up.
SYSTEM_THREAD(ENABLED);

// Enable nice, timestamped logs over USB serial (works in Particle Workbench).
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// Built-in LED on most Particle boards.
constexpr pin_t LED_PIN = D7;

// Cloud publishes are rate limited. This example publishes once every 10 seconds.
constexpr uint32_t PUBLISH_PERIOD_MS = 10 * 1000;

// The event name you will see in the Particle Console event stream.
const char *EVENT_NAME = "training/cloud_publish";

void setup() {
  // Hardware setup.
  pinMode(LED_PIN, OUTPUT);

  Log.info("Boot: cloud publish example");
}

void loop() {
  // 1) Local heartbeat: blink the LED so you can see the firmware is running.
  static uint32_t lastBlink = 0;
  if (millis() - lastBlink >= 1000) {
    lastBlink = millis();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }

  // 2) Cloud heartbeat: publish a small JSON payload periodically.
  static uint32_t lastPublish = 0;
  if (millis() - lastPublish >= PUBLISH_PERIOD_MS) {
    lastPublish = millis();

    // Keep payload small!
    // Particle publishes have size limits, and publishing too often can be throttled.
    char data[64];
    snprintf(data, sizeof(data), "{\"ms\":%lu}", millis());

    // PRIVATE means only your account/devices see it (recommended for training projects).
    const bool ok = Particle.publish(EVENT_NAME, data, PRIVATE);

    // "ok" here only means the device accepted the publish request.
    // If you're offline, it can still return false (and you can try again later).
    Log.info("Publish event='%s' ok=%s data=%s", EVENT_NAME, ok ? "true" : "false", data);
  }
}

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

