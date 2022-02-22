class DEMO_SEQ : public SEQ {
    bool steps [32];
    int pos = 0;
    bool activeNotes[4];
    byte active_channel;

    int mode = 0;

  public:
    void handle (keypadEvent e) {
      if (e.bit.EVENT == KEY_JUST_PRESSED) {
        int r = e.bit.KEY/8;
        int c = e.bit.KEY%8;

        if (e.bit.KEY==7) mode = (mode+1)%4; 
        else steps[e.bit.KEY] = !steps[e.bit.KEY];
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
      canvas->clearPixels();
      if (mode == 0){
        for (int i=0; i<32; i++) canvas->set(i, ((steps[i]) ? scheme.secondary:CANVAS::Color(0,0,0)));
        for (int r = 0; r < 4; r++) canvas->set((r * 8) + pos, ((steps[(r * 8) + pos] == false) ? scheme.main : scheme.highlight));
      }

      if (mode == 1){
        for (int i=0; i<32; i++) canvas->set(i, ((steps[i]) ? scheme.secondary:CANVAS::Color(0,0,0)));
        for (int r = 0; r < 4; r++) canvas->set((r * 8) + pos, ((steps[(r * 8) + pos] == false) ? scheme.main : scheme.highlight));
        canvas->set(0,CANVAS::Color(0,((pos%2==0)?255:128),0));
        for (int i=1; i<8; i++) canvas->set(i, CANVAS::Color(0,0,0));
      }

      if (mode == 2){
        for (int i=0; i<32; i++) canvas->set(i, ((steps[i]) ? scheme.secondary:CANVAS::Color(0,0,0)));
        for (int r = 0; r < 4; r++) canvas->set((r * 8) + pos, ((steps[(r * 8) + pos] == false) ? scheme.main : scheme.highlight));
        canvas->set(0,CANVAS::Color(0,((pos%2==0)?255:128),0));
        for (int i=1; i<8; i++) canvas->set(i, (i==1) ? CANVAS::Color(43,43,43):CANVAS::Color(11,11,11));
      }

      if (mode == 3){
        for (int i=0; i<32; i++) canvas->set(i, ((steps[i]) ? scheme.secondary:CANVAS::Color(0,0,0)));
        for (int r = 0; r < 4; r++) canvas->set((r * 8) + pos, ((steps[(r * 8) + pos] == false) ? scheme.main : scheme.highlight));
        canvas->set(0,CANVAS::Color(0,((pos%2==0)?255:128),0));
        for (int i=1; i<8; i++) canvas->set(i, (i==1) ? CANVAS::Color(43,43,43):CANVAS::Color(11,11,11));
        canvas->set(16,CANVAS::Color(0,0,0));
        canvas->set(17,CANVAS::Color(11,11,11));
        canvas->set(18,CANVAS::Color(11,11,11));
        canvas->set(19,CANVAS::Color(0,0,0));
        canvas->set(20,CANVAS::Color(11,11,11));
        canvas->set(21,CANVAS::Color(11,11,11));
        canvas->set(22,CANVAS::Color(11,11,11));
        canvas->set(23,CANVAS::Color(0,0,0));
        for (int i=24; i<32; i++) canvas->set(i, CANVAS::Color(11,11,11));
      }
    }
};
