/*
https://learn.adafruit.com/adafruit-esp32-feather-v2
https://learn.adafruit.com/assets/123406

Additional URLs:
add
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json

F1 Arduino: Board Manager > esp32

F1 Arduino: Change Board Type > adafruit feather esp32 v2

F1 Arduino: Select Serial Port > ttyACM0
*/

#include <Wire.h>
#include <math.h>

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  analogWriteResolution(12);
}

const int SIZE_X = 256;
const int SIZE_Y = 256;
const float RESOLUTION = 4;

///////////////////////////////////////////////////////////////////////////////
// XY graphic primitives

void draw_point(int x, int y) {
  dacWrite(A0, x >= SIZE_X ? SIZE_X - 1 : x);
  dacWrite(A1, y >= SIZE_Y ? SIZE_Y - 1 : y);
}

void draw_line(int x1, int y1, int x2, int y2) {
  int dx = x2 - x1;
  int dy = y2 - y1;
  float length = sqrt(dx * dx + dy * dy);
  int steps = (int)(length / RESOLUTION + .5);

  float lx = (float)dx / (float)steps;
  float ly = (float)dy / (float)steps;

  for (int i = 0; i < steps; i++) {
    int x = x1 + (int)(lx * i);
    int y = y1 + (int)(ly * i);

    draw_point(x, y);
  }
}

void draw_rect(int x1, int y1, int x2, int y2) {
  draw_line(x1, y1, x2, y1);
  draw_line(x2, y1, x2, y2);
  draw_line(x2, y2, x1, y2);
  draw_line(x1, y2, x1, y1);
}

///////////////////////////////////////////////////////////////////////////////
// Tests

void toggle_max_speed() {
  bool t = false;
  while (true) {
    dacWrite(A1, t ? 0 : 255);
    t = !t;
  }
}

void saw_tooth() {
  uint8_t v = 0;
  while (true) {
    dacWrite(A1, v);
    v++;
  }
}

void lissajous() {
  float t = 0.;

  const float k = 1;
  while (true) {
    float value;

    value = sin(t) / 2. + .5;  // 0..1;
    uint8_t v0 = (uint8_t)(value * 255.);

    value = cos(t * 3.01) / 2. + .5;  // 0..1;
    uint8_t v1 = (uint8_t)(value * 255.);

    draw_point(v0, v1);

    t += .010 * k;
  }
}

////////////////////////////////////////////////////////////////////////////////

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
};

Quix quix = Quix();

void loop() {
  // toggle_max_speed(); // Adafruit Feather ESP32 V2: 26.6 KHz
  // lissajous();

  quix.step();
}
