#include "Particle.h"

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

constexpr pin_t LED_PIN = D7;
constexpr uint32_t SERIAL_PERIOD_MS = 500;

bool ledState = false;

void printHelp() {
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

  Serial.begin(115200);
  waitFor(Serial.isConnected, 5000);

  Serial.println("Boot: training/serial-publishing");
  Serial.println("Type 'help' then press Enter.");
  printHelp();
}

void loop() {
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);

  // Periodic "serial publish" (print structured values over USB serial).
  static uint32_t lastPrint = 0;
  if (millis() - lastPrint >= SERIAL_PERIOD_MS) {
    lastPrint = millis();

    const int raw = analogRead(A0);
    const float v = (raw * 3.3f) / 4095.0f;

    Serial.printf("{\"ms\":%lu,\"a0_raw\":%d,\"a0_v\":%.3f,\"led\":%d}\r\n",
                  millis(), raw, v, ledState ? 1 : 0);
  }

  // Command handling from Serial Monitor (newline-terminated).
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();

    if (cmd.length() == 0) {
      return;
    }

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

