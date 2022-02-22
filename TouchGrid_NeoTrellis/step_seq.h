class STEP_SEQ : public SEQ {
    bool steps [32];
    int pos = 0;
    bool activeNotes[4];
    byte active_channel;

  public:
    void handle (keypadEvent e) {
      if (e.bit.EVENT == KEY_JUST_PRESSED) {
        int r = e.bit.KEY/8;
        int c = e.bit.KEY%8;
        steps[e.bit.KEY] = !steps[e.bit.KEY];
      }
    }
  
    void tick (int p) {
      pos = p;
      // old Notes off
      for (int r = 0; r < 4; r++) {
        if (activeNotes[r]) {
          activeNotes[r] = false;
          noteOff(active_channel,39-r,0);
        }
      }
      // new Notes on
      for (int r = 0; r < 4; r++) {
        if (steps[(r*8)+pos]) {
          activeNotes[r] = true;
          active_channel = channel;
          noteOn(channel,39-r,100);
        }
      }
    }

    void plot () {
      if (canvas==NULL) return;
      for (int i=0; i<32; i++) canvas->set(i, ((steps[i]) ? scheme.secondary:CANVAS::Color(0,0,0)));
      //for (int r = 0; r < 4; r++) canvas->set((r * 8) + pos, scheme.main);
      for (int r = 0; r < 4; r++) canvas->set((r * 8) + pos, ((steps[(r * 8) + pos] == false) ? scheme.main : scheme.highlight));
    }
};
