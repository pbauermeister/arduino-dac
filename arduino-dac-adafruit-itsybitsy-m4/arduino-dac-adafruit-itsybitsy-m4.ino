/*
Purpose
-------
Oscilloscope vector graphics: Board generates analog X and Y signals, and
oscilloscope displays them in XY mode. Also use Z for blanking.

ItsyBitsy setup
---------------
https://learn.adafruit.com/introducing-adafruit-itsybitsy-m4/setup
F1 Arduino: Board Manager > SAMD
F1 Arduino: Change Board Type > Adafruit ItsyBitsy M4 (SAMD51)
F1 Arduino: Select Serial Port > ttyACM0
F1 Arduino: Library Manager > Adafruit DotStar by Adafruit

Noise
-----
https://forum.arduino.cc/t/horrific-dac-noise-on-due-board/189749/14
https://forum.arduino.cc/t/noise-on-external-dac-when-reading-sd-card/1156137/10
https://www.electrosmash.com/forum/hardware-pedalshield/26-powering-arduino-due
Ideas:
- Decoupling capacitor: Place capacitors (e.g., 0.1µF and 10µF)
  close to the DAC’s power supply pins to filter out high-frequency noise.
  => between GND and USB/3V/AREF => no good
- Use PSU
- Use batt

*/

#include <Adafruit_DotStar.h>
#include <Wire.h>
#include <math.h>

////////////////////////////////////////////////////////////////////////////////
// Board-specific stuff
////////////////////////////////////////////////////////////////////////////////

//
// mapping and spans
//

#define X_DAC_PORT A0              // analog out (true DAC)
#define Y_DAC_PORT A1              // analog out (true DAC)
#define Z_BLANKING_PORT A2         // digital out
const int DAC_SPAN = 4096;         // DAC has 12 bits
const float LINE_RESOLUTION = 64;  // bigger -> coarser, needed if BW is too low

//
// On-board RGB LED
//

Adafruit_DotStar strip(1, 8, 6, DOTSTAR_RGB);

void init_led() {
  strip.begin();
  strip.setPixelColor(0, 1, 0, 2);
  strip.show();
}

//
// DAC and blanking output
//

void dac_write_x(uint32_t value) { analogWrite(X_DAC_PORT, value); }
void dac_write_y(uint32_t value) { analogWrite(Y_DAC_PORT, value); }
void pen_up() { digitalWrite(Z_BLANKING_PORT, HIGH); }
void pen_down() { digitalWrite(Z_BLANKING_PORT, LOW); }

////////////////////////////////////////////////////////////////////////////////

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(Z_BLANKING_PORT, OUTPUT);
  digitalWrite(Z_BLANKING_PORT, HIGH);

  analogWriteResolution(12);
  init_led();
  pen_up();
}

////////////////////////////////////////////////////////////////////////////////
// Derived constants
////////////////////////////////////////////////////////////////////////////////

#define MAX_VAL (DAC_SPAN - 1)

const int SIZE_X = DAC_SPAN - 1;
const int SIZE_Y = DAC_SPAN - 1;

///////////////////////////////////////////////////////////////////////////////
// XY graphic primitives
////////////////////////////////////////////////////////////////////////////////

void draw_point(int x, int y, bool set_pen = true) {
  dac_write_x(x >= SIZE_X ? SIZE_X - 1 : x);
  dac_write_y(y >= SIZE_Y ? SIZE_Y - 1 : y);
  if (set_pen) {
    pen_down();
    pen_up();
  }
}

void draw_line(int x1, int y1, int x2, int y2, bool set_pen = true) {
  int dx = x2 - x1;
  int dy = y2 - y1;
  float length = sqrt(dx * dx + dy * dy);
  int steps = (int)(length / LINE_RESOLUTION + .5);

  float lx = (float)dx / (float)steps;
  float ly = (float)dy / (float)steps;

  if (set_pen) pen_down();
  for (int i = 0; i < steps; i++) {
    int x = x1 + (int)(lx * i);
    int y = y1 + (int)(ly * i);

    draw_point(x, y, false);
  }
  if (set_pen) pen_up();
}

void draw_rect(int x1, int y1, int x2, int y2) {
  pen_down();
  draw_line(x1, y1, x2, y1, false);
  draw_line(x2, y1, x2, y2, false);
  draw_line(x2, y2, x1, y2, false);
  draw_line(x1, y2, x1, y1, false);
  pen_up();
}

///////////////////////////////////////////////////////////////////////////////
// Tests
///////////////////////////////////////////////////////////////////////////////

void toggle_max_speed() {
  bool t = false;
  while (true) {
    dac_write_x(t ? 0 : DAC_SPAN - 1);
    t = !t;
  }
}

void saw_tooth() {
  uint16_t v = 0;
  while (true) {
    dac_write_x(v);
    v++;
    v = v % DAC_SPAN;
  }
}

void square() {
  bool b = false;
  while (true) {
    dac_write_x(b ? DAC_SPAN - 1 : 0);
    b = !b;
    delay(50);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Lissajous
///////////////////////////////////////////////////////////////////////////////

void lissajous() {
  float t = 0.;

  const float k = 1;
  pen_down();
  while (true) {
    float value;

    value = sin(t) / 2. + .5;  // 0..1;
    uint16_t v0 = (uint16_t)(value * MAX_VAL);

    value = cos(t * 3.01) / 2. + .5;  // 0..1;
    uint16_t v1 = (uint16_t)(value * MAX_VAL);

    draw_point(v0, v1, false);

    t += .010 * k;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Quix
///////////////////////////////////////////////////////////////////////////////

class Line {
 public:
  float x0;
  float y0;
  float x1;
  float y1;

 private:
  bool set;

 public:
  Line(float px0, float py0, float px1, float py1) {
    x0 = px0;
    y0 = py0;
    x1 = px1;
    y1 = py1;
    set = true;
  }
  Line() { set = false; }

  void copy_from(Line &other) {
    x0 = other.x0;
    y0 = other.y0;
    x1 = other.x1;
    y1 = other.y1;
    set = true;
  }

  void draw() {
    if (!set) return;
    draw_line(x0, y0, x1, y1);
  }
};

class Quix {
 private:
  static const int STEP = 14;
  static const int TRAIL_SIZE = 12;

  Line head_line =
      Line(random(SIZE_X), random(SIZE_Y), random(SIZE_X), random(SIZE_Y));

  Line trail_lines[TRAIL_SIZE];
  int trail_head_index = 0;

  float mk_step(bool sgn = false) {
    float val = ((float)random(STEP * 10) + STEP) / 10.;
    if (sgn && random(2) == 0) val = -val;
    return val;
  }

  Line speed = Line(mk_step(true), mk_step(true), mk_step(true), mk_step(true));

 public:
  Quix() {}

  void step() {
    // grow trail
    trail_lines[trail_head_index].copy_from(head_line);

    // draw trail
    for (int i = 0; i < TRAIL_SIZE; i++) {
      trail_lines[(trail_head_index + i) % TRAIL_SIZE].draw();
    }

    // draw box
    draw_rect(0, 0, SIZE_X - 1, SIZE_Y - 1);

    // advance head
    trail_head_index = (trail_head_index + 1) % TRAIL_SIZE;

    // advance line
    head_line.x0 += speed.x0;
    if (head_line.x0 < 0) {
      head_line.x0 = 0;
      speed.x0 = mk_step();
    } else if (head_line.x0 >= SIZE_X) {
      head_line.x0 = SIZE_X - 1;
      speed.x0 = -mk_step();
    }

    head_line.x1 += speed.x1;
    if (head_line.x1 < 0) {
      head_line.x1 = 0;
      speed.x1 = mk_step();
    } else if (head_line.x1 >= SIZE_X) {
      head_line.x1 = SIZE_X - 1;
      speed.x1 = -mk_step();
    }

    head_line.y0 += speed.y0;
    if (head_line.y0 < 0) {
      head_line.y0 = 0;
      speed.y0 = mk_step();
    } else if (head_line.y0 >= SIZE_Y) {
      head_line.y0 = SIZE_Y - 1;
      speed.y0 = -mk_step();
    }

    head_line.y1 += speed.y1;
    if (head_line.y1 < 0) {
      head_line.y1 = 0;
      speed.y1 = mk_step();
    } else if (head_line.y1 >= SIZE_Y) {
      head_line.y1 = SIZE_Y - 1;
      speed.y1 = -mk_step();
    }
  }

  void run() {
    for (;;) step();
  }
};

///////////////////////////////////////////////////////////////////////////////
// Here it goes
///////////////////////////////////////////////////////////////////////////////

Quix quix = Quix();

void loop() {
  // toggle_max_speed();  // Adafruit Itsy Bitsy M4: 71.2 kHz
  // saw_tooth();
  // square();
  lissajous();

  // quix.run();
}
