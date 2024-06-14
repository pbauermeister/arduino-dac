
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
#include <math.h>

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

int dt = 2;

float ky = 2.1;
float dy = 0;  // PI / 2.;

int t = 0;

void loop() {
  float tx = (float)t / 360. * 2. * PI;

  float xf = (sin(tx) / 2. + 0.5) * (SPAN - 1);
  int x = (int)xf;

  float ty = tx * ky + dy;
  float yf = (sin(ty) / 2. + 0.5) * (SPAN - 1);
  int y = (int)yf;

  //  mcp.setChannelValue(MCP4728_CHANNEL_A, x);
  //  mcp.setChannelValue(MCP4728_CHANNEL_B, y);
  mcp.fastWrite(x, y, 0, 0);

  t += dt;
  if ((t % 360) == 0) digitalWrite(LED_BUILTIN, HIGH);
  if ((t % 360) == 180) digitalWrite(LED_BUILTIN, LOW);
}
