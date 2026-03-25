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

// Device OS 6.2.0+ enables system threading by default.
// For training, we can rely on the default behavior and avoid warnings.

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
