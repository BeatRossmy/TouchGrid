enum DIRECTION {UP, DOWN, LEFT, RIGHT};

struct CANVAS {
  uint32_t pixel [32];

  static uint32_t   Color(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
  }

  void set (int index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= 32) return;
    pixel[index] = Color(r, g, b);
  }

  void set (int index, uint32_t rgb) {
    if (index >= 32) return;
    pixel[index] = rgb;
  }

  uint32_t get (int index) {
    if (index >= 32) return 0;
    return pixel[index];
  }

  void clearPixels () {
    for (int i = 0; i < 32; i++) pixel[i] = Color(0, 0, 0);
  }

  bool equalsAt (CANVAS* c, int index) {
    if (index >= 32) return false;
    return (c->pixel[index] == pixel[index]);
  }

  void show (Adafruit_NeoPixel* strip) {
    for (int i = 0; i < 32; i++) {
        strip->setPixelColor(i, pixel[i]);
    }
    strip->show();
  }

  void plot (CANVAS* c, Adafruit_NeoPixel* strip) {
    for (int i = 0; i < 32; i++) {
      if (!equalsAt(c, i)) {
        pixel[i] = c->get(i);
        strip->setPixelColor(i, pixel[i]);
      }
    }
    strip->show();
  }

  void setAndSend (int index, uint32_t color, Adafruit_NeoPixel* strip) {
    if (pixel[index]!=color) {
      pixel[index] = color;
      strip->setPixelColor(index, color);
    }
  }

  void plot (CANVAS* a, CANVAS* b, int xFade, Adafruit_NeoPixel* strip) {
    for (int i = 0; i < 32; i++) {
      if (i % 8 >= xFade) {
        pixel[i - xFade] = a->get(i);
        strip->setPixelColor(i - xFade, a->get(i));
      }
      else {
        pixel[i + (8 - xFade)] = b->get(i);
        strip->setPixelColor(i + (8 - xFade), b->get(i));
      }
    }
    strip->show();
  }

  /*
   * xFade == 0 -> 100% old_c
   * xFade == 8 -> 100% new_c
  */
  void plot (CANVAS* old_c, CANVAS* new_c, DIRECTION dir, int xFade, Adafruit_NeoPixel* strip) {
    if (dir == LEFT) {
      for (int i = 0; i < 32; i++) {
        if (i % 8 >= xFade) {
          uint32_t color = old_c->get(i);
          setAndSend(i - xFade,color,strip);
        }
        else {
          uint32_t color = new_c->get(i);
          setAndSend(i + (8 - xFade),color,strip);
        }
      }
    }

    else if (dir == RIGHT) {
      for (int i = 0; i < 32; i++) {
        if (i % 8 < 8 - xFade) {
          uint32_t color = old_c->get(i);
          setAndSend(i + xFade,color,strip);
        }
        else {
          uint32_t color = new_c->get(i);
          setAndSend(i - (8 - xFade),color,strip);
        }
      }
    }

    else if (dir == DOWN) {
      for (int i = 0; i < 32; i++) {
        if (i / 8 < (4 - xFade/2)) {
          uint32_t color = old_c->get(i);
          setAndSend(i + (8*(xFade/2)),color,strip);
        }
        else {
          uint32_t color = new_c->get(i);
          setAndSend(i - (8*(4-xFade/2)),color,strip);
        }
      }
    }

    else if (dir == UP) {
      for (int i = 0; i < 32; i++) {
        if (i / 8 >= xFade/2) {
          uint32_t color = old_c->get(i);
          setAndSend(i - (8*(xFade/2)),color,strip);
        }
        else {
          uint32_t color = new_c->get(i);
          setAndSend(i + (8*(4-xFade/2)),color,strip);
        }
      }
    }
    
    strip->show();
  }
};
