
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
  Wire.setClock(1000000);

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

float ky = 2.1;

int t = 0;
int dt = 1;

int i = 0;

void fast_square() {
  mcp.setChannelValue(MCP4728_CHANNEL_A, 0);
  mcp.setChannelValue(MCP4728_CHANNEL_B, 0);

  mcp.setChannelValue(MCP4728_CHANNEL_A, 4095);
  mcp.setChannelValue(MCP4728_CHANNEL_B, 4095);
}

void lissajous() {
  float tx = (float)t / 360. * 2. * PI;

  float xf = (sin(tx) / 2. + 0.5) * (SPAN - 1);
  int x = (int)xf;

  ky = 1.5 + sin(tx / 100);

  float ty = tx * ky;
  float yf = (sin(ty) / 2. + 0.5) * (SPAN - 1);
  int y = (int)yf;

  mcp.setChannelValue(MCP4728_CHANNEL_A, x);
  mcp.setChannelValue(MCP4728_CHANNEL_B, y);
}

void loop() {
  // fast_square();
  lissajous();

  t += dt;

  i++;

  return;
  const int PERIOD = 20;
  if ((i % PERIOD) == 0) digitalWrite(LED_BUILTIN, HIGH);
  if ((i % PERIOD) == PERIOD / 2) digitalWrite(LED_BUILTIN, LOW);
}
