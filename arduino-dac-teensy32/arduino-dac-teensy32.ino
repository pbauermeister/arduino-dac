
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

void mcp4728_square() {
  mcp.setChannelValue(MCP4728_CHANNEL_A, 0);
  mcp.setChannelValue(MCP4728_CHANNEL_B, 0);

  mcp.setChannelValue(MCP4728_CHANNEL_A, 4095);
  mcp.setChannelValue(MCP4728_CHANNEL_B, 4095);
}

void teensy_a14_square() {
  // => 470 kHz
  analogWriteResolution(12);
  while (true) {
    digitalWrite(LED_BUILTIN, HIGH);
    analogWrite(A14, 0);

    digitalWrite(LED_BUILTIN, LOW);
    analogWrite(A14, 4095);
  }
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

const int RESOLUTION = 70;
const int STEP = 100;
const int SIZEX = 4096;
const int SIZEY = (4096 * 3) / 4;

void pen(bool down) {
  // digitalWrite(LED_BUILTIN, !down ? HIGH : LOW);
}

void line(int x1, int y1, int x2, int y2) {
  int dx = x2 - x1;
  int dy = y2 - y1;
  float length = sqrt(dx * dx + dy * dy);
  int steps = (int)(length / RESOLUTION + .5);

  float lx = dx / steps;
  float ly = dy / steps;

  for (int i = 0; i < steps; i++) {
    int x = x1 + (int)(lx * i);
    int y = y1 + (int)(ly * i);

    if (i == 0) pen(true);
    mcp.setChannelValue(MCP4728_CHANNEL_A, x);
    mcp.setChannelValue(MCP4728_CHANNEL_B, y);
  }
  pen(false);
}

int px0 = random(SIZEX);
int py0 = random(SIZEY);
int px1 = random(SIZEX);
int py1 = random(SIZEY);

int mk_step(bool sgn = false) {
  int val = random(STEP / 4) + STEP * 2;
  if (sgn && random(2) == 0) val = -val;
  return val;
}

int dx0 = mk_step(true);
int dy0 = mk_step(true);
int dx1 = mk_step(true);
int dy1 = mk_step(true);

void quix() {
  line(px0, py0, px1, py1);
  line(SIZEX - px0, SIZEY - py0, SIZEX - px1, SIZEY - py1);

  px0 += dx0;
  if (px0 < 0) {
    px0 = 0;
    dx0 = mk_step();
  } else if (px0 >= SIZEX) {
    px0 = SIZEX - 1;
    dx0 = -mk_step();
  }

  px1 += dx1;
  if (px1 < 0) {
    px1 = 0;
    dx1 = mk_step();
  } else if (px1 >= SIZEX) {
    px1 = SIZEX - 1;
    dx1 = -mk_step();
  }

  py0 += dy0;
  if (py0 < 0) {
    py0 = 0;
    dy0 = mk_step();
  } else if (py0 >= SIZEY) {
    py0 = SIZEY - 1;
    dy0 = -mk_step();
  }

  py1 += dy1;
  if (py1 < 0) {
    py1 = 0;
    dy1 = mk_step();
  } else if (py1 >= SIZEY) {
    py1 = SIZEY - 1;
    dy1 = -mk_step();
  }
}

void loop() {
  // quix();

  mcp4728_square();  // ==> 650 Hz
  // teensy_a14_square(); // ==> 470 kHz
  //  lissajous();
  return;

  t += dt;

  i++;

  const int PERIOD = 20;
  if ((i % PERIOD) == 0) digitalWrite(LED_BUILTIN, HIGH);
  if ((i % PERIOD) == PERIOD / 2) digitalWrite(LED_BUILTIN, LOW);
}
