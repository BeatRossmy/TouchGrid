struct DROP {
  int pitch;
  int height;
  int pos;
  bool active;
  bool active_note;
  byte active_channel;

  void set (int r) {
    if (active && height == 4 - r) active = false;
    else {
      height = 4 - r;
      pos = 0;
      active = true;
    }
  }

  void tick (byte channel) {
    if (active_note) {
      active_note = false;
      noteOff(active_channel, pitch, 0);
    }

    if (active) {
      pos = (pos + 1) % height;
      if (pos == height - 1) {
        active_note = true;
        active_channel = channel;
        noteOn(channel, pitch, 100);
      }
    }
  }
};

class DROP_SEQ : public SEQ {
    // c d f g a c d f
    DROP drops [8] = {{60,2}, {62,2}, {65,2}, {67,2}, {69,2}, {72,2}, {74,2}, {77,2}};

  public:
  
    void handle (keypadEvent e) {
      if (e.bit.EVENT == KEY_JUST_PRESSED) {
        int r = e.bit.KEY / 8;
        int c = e.bit.KEY % 8;
        drops[c].set(r);
      }
    }

    void tick (int p) {
      for (int d = 0; d < 8; d++) drops[d].tick(channel);
    }

    void plot () {
      if (canvas==NULL) return;
      canvas->clearPixels();
      for (int c = 0; c < 8; c++) {
        canvas->set((4 - drops[c].height) * 8 + c, scheme.secondary);
        if (drops[c].active) canvas->set((4 - drops[c].height + drops[c].pos) * 8 + c, ((drops[c].active_note)?scheme.highlight:scheme.main));
      }
    }
};
