
/*
Additional URL: https://www.pjrc.com/teensy/package_teensy_index.json

F1 Arduino: Board Manager > teensy > Teensy (for Arduino IDE 2.0.4 or later) by
Paul Stoffregen

F1 Arduino: Change Board Type > Teensy 3.2/3.1
    "board": "arduino:mbed_rp2040:pico",


wget https://www.pjrc.com/teensy/00-teensy.rules
sudo mv 00-teensy.rules /etc/udev/rules.d/
sudo service udev restart

F1 Arduino: Select Serial Port > ttyACM0

F1 Arduino: Library Manager > Adafruit MCP4728 by Adafruit
*/

// #include <Adafruit_MCP4728.h>

#include <Wire.h>

#include "mcp4728.h"

#if 0

void setVal(int v) {
  mcp.setChannelValue(MCP4728_CHANNEL_A, v);
  /*
    mcp.setChannelValue(MCP4728_CHANNEL_B, 2048);
    mcp.setChannelValue(MCP4728_CHANNEL_C, 1024);
    mcp.setChannelValue(MCP4728_CHANNEL_D, 0);
  */
}

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

int v = 0;

void loop() {
  mcp.setChannelValue(MCP4728_CHANNEL_A, v);
  v += 32;
  v = v % 4096;

  if (v == 0) digitalWrite(LED_BUILTIN, HIGH);
  if (v == 1024) digitalWrite(LED_BUILTIN, LOW);
}

#endif

///////////////////

#include <Arduino.h>

REDIRECT_STDOUT_TO(Serial);

MCP4728 dac;

void blink(int n = -1, int d = 300) {
  while (n--) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(d);
    digitalWrite(LED_BUILTIN, LOW);
    delay(d);
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);  // initialize serial interface for print()

  while (true) {
    blink(2);
    Serial.println("Hello world!");
    delay(1000);
  }
  Wire.begin();
  dac.attach(Wire, 14);
  dac.readRegisters();

  dac.selectVref(MCP4728::VREF::VDD, MCP4728::VREF::VDD,
                 MCP4728::VREF::INTERNAL_2_8V, MCP4728::VREF::INTERNAL_2_8V);
  dac.selectPowerDown(
      MCP4728::PWR_DOWN::GND_100KOHM, MCP4728::PWR_DOWN::GND_100KOHM,
      MCP4728::PWR_DOWN::GND_500KOHM, MCP4728::PWR_DOWN::GND_500KOHM);
  dac.selectGain(MCP4728::GAIN::X1, MCP4728::GAIN::X1, MCP4728::GAIN::X2,
                 MCP4728::GAIN::X2);
  blink(0);

  dac.analogWrite(MCP4728::DAC_CH::A, 111);
  dac.analogWrite(MCP4728::DAC_CH::B, 222);
  dac.analogWrite(MCP4728::DAC_CH::C, 333);
  dac.analogWrite(MCP4728::DAC_CH::D, 444);

  int v = 0;

  while (true) {
    dac.analogWrite(MCP4728::DAC_CH::A, v);
    v += 32;
    v = v % 4096;

    if (v == 0) digitalWrite(LED_BUILTIN, HIGH);
    if (v == 1024) digitalWrite(LED_BUILTIN, LOW);
  }

  /*

  dac.selectVref(MCP4728::VREF::VDD, MCP4728::VREF::VDD,
                 MCP4728::VREF::INTERNAL_2_8V, MCP4728::VREF::INTERNAL_2_8V);
  dac.selectPowerDown(
      MCP4728::PWR_DOWN::GND_100KOHM, MCP4728::PWR_DOWN::GND_100KOHM,
      MCP4728::PWR_DOWN::GND_500KOHM, MCP4728::PWR_DOWN::GND_500KOHM);
  dac.selectGain(MCP4728::GAIN::X1, MCP4728::GAIN::X1, MCP4728::GAIN::X2,
                 MCP4728::GAIN::X2);
  dac.analogWrite(MCP4728::DAC_CH::A, 111);
  dac.analogWrite(MCP4728::DAC_CH::B, 222);
  dac.analogWrite(MCP4728::DAC_CH::C, 333);
  dac.analogWrite(MCP4728::DAC_CH::D, 444);

  dac.enable(true);

  dac.readRegisters();
  printStatus();
  delay(5000);
  */
}
void loop() {
  return;
  /*
    static int count = 0;
    if (++count > 4000) count = 0;

    dac.analogWrite(count, count, count, count);

    delay(10);
    dac.readRegisters();
    printStatus();
  */
}
/*

void printStatus() {
  Serial.println("NAME     Vref  Gain  PowerDown  DACData");
  for (int i = 0; i < 4; ++i) {
    Serial.print("DAC");
    Serial.print(i, DEC);
    Serial.print("   ");
    Serial.print("    ");
    Serial.print(dac.getVref(i), BIN);
    Serial.print("     ");
    Serial.print(dac.getGain(i), BIN);
    Serial.print("       ");
    Serial.print(dac.getPowerDown(i), BIN);
    Serial.print("       ");
    Serial.println(dac.getDACData(i), DEC);

    Serial.print("EEPROM");
    Serial.print(i, DEC);
    Serial.print("    ");
    Serial.print(dac.getVref(i, true), BIN);
    Serial.print("     ");
    Serial.print(dac.getGain(i, true), BIN);
    Serial.print("       ");
    Serial.print(dac.getPowerDown(i, true), BIN);
    Serial.print("       ");
    Serial.println(dac.getDACData(i, true), DEC);
  }
  Serial.println(" ");
}
*/
