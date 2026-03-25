#include "Particle.h"

// =========================
// Branch: training/serial-publishing
// Concept: Serial publishing (printing data over USB Serial)
//
// What you learn here:
// - How to print telemetry data to a Serial Monitor (USB)
// - How to format data as easy-to-parse JSON lines
// - How to read simple commands from Serial (on/off/toggle)
//
// How to verify:
// - Open a Serial Monitor at 115200 baud
// - You should see JSON lines printing periodically
// - Type commands like: on, off, toggle, help
// =========================

// Run user code even while connecting to the cloud.
SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

// Built-in LED on most Particle boards.
constexpr pin_t LED_PIN = D7;

// How often we print one line of telemetry.
constexpr uint32_t SERIAL_PERIOD_MS = 500;

// Our "device state" — we will print this and also allow Serial commands to change it.
bool ledState = false;

void printHelp() {
  // A small "menu" so learners can discover the commands.
  Serial.println();
  Serial.println("Serial commands:");
  Serial.println("  on       -> LED on");
  Serial.println("  off      -> LED off");
  Serial.println("  toggle   -> LED toggle");
  Serial.println("  help     -> show this help");
  Serial.println();
}

void setup() {
  pinMode(LED_PIN, OUTPUT);

  // Start USB serial. Baud rate must match your Serial Monitor setting.
  Serial.begin(115200);

  // Optional: wait a moment for the PC to open the serial connection.
  // If you don't wait, your first few prints might be missed.
  waitFor(Serial.isConnected, 5000);

  Serial.println("Boot: training/serial-publishing");
  Serial.println("Type 'help' then press Enter.");
  printHelp();
}

void loop() {
  // Apply the state to the actual LED pin.
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);

  // Periodic "serial publish" (print structured values over USB serial).
  static uint32_t lastPrint = 0;
  if (millis() - lastPrint >= SERIAL_PERIOD_MS) {
    lastPrint = millis();

    // Example sensor read: A0 analog input.
    // Note: the ADC range and reference voltage can vary by device.
    const int raw = analogRead(A0);

    // Many Particle devices use a 12-bit ADC (0..4095) with ~3.3V reference.
    // If your board differs, this voltage calculation may need adjusting.
    const float v = (raw * 3.3f) / 4095.0f;

    // Print ONE JSON object per line.
    // This is easy to parse later if you want to pipe it into a script or logger.
    // We use \r\n so it looks correct in most serial monitors.
    Serial.printf("{\"ms\":%lu,\"a0_raw\":%d,\"a0_v\":%.3f,\"led\":%d}\r\n",
                  millis(), raw, v, ledState ? 1 : 0);
  }

  // Command handling from Serial Monitor (newline-terminated).
  if (Serial.available()) {
    // Read until Enter/newline is pressed in the Serial Monitor.
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();

    // Ignore empty lines (for example, if the user just pressed Enter).
    if (cmd.length() == 0) {
      return;
    }

    // Simple command parser.
    if (cmd == "on") {
      ledState = true;
      Serial.println("OK: LED on");
    } else if (cmd == "off") {
      ledState = false;
      Serial.println("OK: LED off");
    } else if (cmd == "toggle") {
      ledState = !ledState;
      Serial.println("OK: LED toggled");
    } else if (cmd == "help" || cmd == "?") {
      printHelp();
    } else {
      Serial.println("ERR: unknown command (try 'help')");
    }
  }
}

