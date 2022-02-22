#include <elapsedMillis.h>

// Bestimmt den Hardware Serialport
#define HWSERIAL Serial1
#define SERIAL_BUFFER_SIZE 100

#include <Adafruit_NeoPixel.h>
#define displayPin 10
#include <Adafruit_NeoTrellisM4.h>

//#define MIDI_CHANNEL     0

Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();
Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, displayPin, NEO_GRB + NEO_KHZ800);

#include "canvas.h"

#include "sequencer.h"

#include "menu.h"

#include "demo_seq.h"
#include "step_seq.h"
#include "drop_seq.h"
#include "random_walk_seq.h"

bool demoMode = false;

DEMO_SEQ sqd; 


STEP_SEQ sq1;
DROP_SEQ sq2;
RANDOM_WALK_SEQ sq3;

MENU menu;



static const int sq_length = 3;
SEQ* sq_list[sq_length] = {&sq1, &sq2, &sq3};
int sq_ptr = 0;

SEQ* active_sq = sq_list[sq_ptr];
SEQ* last_sq = sq_list[sq_ptr]; // egal welcher

int clock_pos;
elapsedMillis kRate;
elapsedMillis fadeRate;
elapsedMillis frameRate;
elapsedMillis block_touch;
bool pause = false;

int wait_counter;
char msg;

CANVAS main_canvas;
CANVAS canvas_last, canvas_active;
DIRECTION xDir = LEFT;
int xFade = 8;
int xSpeed = 1;

void setup() {
  Serial.begin(9600);
  HWSERIAL.begin(9600);

  trellis.begin();

  strip.begin();
  strip.show();
  strip.setBrightness(50);

  sq1.setChannel(0);
  sq2.setChannel(1);
  sq3.setChannel(2);

  
  sqd.setColors(CANVAS::Color(16, 16, 16), CANVAS::Color(0, 0, 128), CANVAS::Color(32, 32, 255), CANVAS::Color(0, 0, 32));
  
  sq1.setColors(CANVAS::Color(16, 16, 16), CANVAS::Color(0, 0, 128), CANVAS::Color(32, 32, 255), CANVAS::Color(0, 0, 32));
  sq2.setColors(CANVAS::Color(16, 16, 16), CANVAS::Color(64, 64, 0), CANVAS::Color(160, 160, 32), CANVAS::Color(16, 16, 0));
  sq3.setColors(CANVAS::Color(16, 16, 16), CANVAS::Color(43, 43, 43), CANVAS::Color(255, 255, 255), CANVAS::Color(11, 11, 11));

  active_sq->setCanvas(&canvas_active);

  menu.setPause(&pause);

  kRate = 0;
}

void loop() {
  readSerial();
  handleTrellis();
  handleReceivedMsg();

  // PLOT
  if (fadeRate > 40) {
    fadeRate -= 40;
    if (xSpeed != 0 && (xFade > 0 || xFade < 8)) {
      xFade += xSpeed;
      xFade = constrain(xFade, 0, 8);
    }
  }

  //
  if (frameRate > 20) {
    frameRate -= 20;
    if (demoMode) {
      sqd.setCanvas(&main_canvas);
      sqd.plot();
      main_canvas.show(&strip);
    }
    else {
      /*if (menu.active)*/ menu.plot();
      if (active_sq != NULL) active_sq->plot();
      if (last_sq != NULL) last_sq->plot();
  
      main_canvas.plot(&canvas_last, &canvas_active, xDir, (int)xFade, &strip);
    }
  }

  // CLOCK
  if (kRate > 200) {
    kRate -= 200;
    if (!pause) {
      clock_pos = (clock_pos + 1) % 8;
      for (int s = 0; s < sq_length; s++) sq_list[s]->tick(clock_pos);
      menu.tick(clock_pos);
      sqd.tick(clock_pos);
    }
  }
}

void handleReceivedMsg () {
  if (demoMode) return;
  
  if (block_touch > 300 && msg != ' ') {
    Serial.println(msg);

    if (msg == 't') {
      //pause = !pause;

      menu.active = !menu.active;

      if (menu.active) {
        last_sq = active_sq;
        active_sq = NULL;

        menu.setSeq(last_sq);

        last_sq->setCanvas(&canvas_last);
        menu.setCanvas(&canvas_active);

        xFade = 1;
        xDir = DOWN;
      }
      else {
        active_sq = last_sq;
        last_sq = NULL;

        active_sq->setCanvas(&canvas_active);
        menu.setCanvas(&canvas_last);

        xFade = 1;
        xDir = UP;
      }
    }

    if (!menu.active) {

      if (msg == 'r') {
        if (sq_ptr > 0) {
          menu.active = false;
          sq_ptr--;
          last_sq = active_sq;
          active_sq = sq_list[sq_ptr];

          last_sq->setCanvas(&canvas_last);
          active_sq->setCanvas(&canvas_active);
          xFade = 1;
          xDir = RIGHT;
        }
      }

      if (msg == 'l') {
        if (sq_ptr < 2) {
          menu.active = false;
          sq_ptr++;
          last_sq = active_sq;
          active_sq = sq_list[sq_ptr];

          last_sq->setCanvas(&canvas_last);
          active_sq->setCanvas(&canvas_active);
          xFade = 1;
          xDir = LEFT;
        }
      }
      
    }
    msg = ' ';
  }
}

void handleTrellis () {
  trellis.tick();
  while (trellis.available()) {
    keypadEvent e = trellis.read();
    Serial.println("key");
    block_touch = 0;
    msg = ' '; // delete open msgs
    if (demoMode) sqd.handle(e);
    else if (active_sq != NULL) active_sq->handle(e);
    else if (menu.active) menu.handle(e);
  }
}

void readSerial () {
  char incomingByte;
  bool flag = false;

  while (HWSERIAL.available() > 0) {
    incomingByte = HWSERIAL.read();
    if (msg!='l') msg = incomingByte;
    Serial.println("touch");
    block_touch = 0;
  }
}
