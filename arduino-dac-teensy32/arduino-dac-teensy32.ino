
/*
Additional URL: https://www.pjrc.com/teensy/package_teensy_index.json

F1 Arduino: Board Manager > teensy > Teensy (for Arduino IDE 2.0.4 or later) by
Paul Stoffregen

F1 Arduino: Change Board Type > Teensy 3.2/3.1

wget https://www.pjrc.com/teensy/00-teensy.rules
sudo mv 00-teensy.rules /etc/udev/rules.d/
sudo service udev restart

F1 Arduino: Select Serial Port > ttyACM0

F1 Arduino: Library Manager > Adafruit MCP4728 by Adafruit
*/

#include <Adafruit_MCP4728.h>
#include <Wire.h>

Adafruit_MCP4728 mcp;

bool initMcp() {
  if (!mcp.begin()) {
    Serial.println("Failed to find MCP4728 chip");
    return false;
  }
  return true;
}

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Adafruit MCP4728 test!");
  while (!initMcp()) delay(300);
}

const int SPAN = 4096;
const int K = 2;

int x = 0;
int dx = 32 * K;

int y = SPAN / 2;
int dy = 32 * K;

void loop() {
  mcp.setChannelValue(MCP4728_CHANNEL_A, x);
  x += dx;
  x = x % SPAN;

  mcp.setChannelValue(MCP4728_CHANNEL_B, y);
  y += dy;
  y = y % SPAN;

  if (x == 0) digitalWrite(LED_BUILTIN, HIGH);
  if (x == 1024) digitalWrite(LED_BUILTIN, LOW);
}
