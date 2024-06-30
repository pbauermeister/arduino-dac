
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

#include <Wire.h>
#include <math.h>

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  analogWriteResolution(12);
  // Serial.begin(115200);
}

float t = 0.;
const float HZ = 100;
const float k = 1 / 9.97 * HZ;
const float dt = 1. / 360. * PI * 2. * k;
int i = 0;
bool b = false;

void teensy_a14_square() {
  float lv0 = 0;
  while (true) {
    float v0 = sin(t);
    float vv = v0 / 2. + .5;
    int v = (int)(vv * 4095.);
    analogWrite(A14, v);
    t += dt;
    i++;

    if ((v0 > 0) != (lv0 > 0)) {
      b = !b;
      digitalWrite(LED_BUILTIN, b ? HIGH : LOW);
    }

    lv0 = v0;
  }
}

void loop() {
  teensy_a14_square();  // ==> 470 kHz
  return;
}
