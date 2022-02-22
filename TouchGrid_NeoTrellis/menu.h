class MENU {
  public:
    CANVAS* canvas = NULL;
    SEQ* seq = NULL;

    bool* pause;

    bool active;

    int pos;
    
    void setCanvas (CANVAS* c) {canvas = c;}
    
    virtual void handle (keypadEvent e) {
      if (e.bit.EVENT == KEY_JUST_PRESSED) {
        int r = e.bit.KEY/8;
        int c = e.bit.KEY%8;
        
        if (e.bit.KEY == 1 || e.bit.KEY == 9 || e.bit.KEY == 10 || e.bit.KEY == 17) {
          *pause = !*pause; 
        }
        if (e.bit.KEY%8>3) {
          seq->setChannel(c-4+r*4);
        }
      }  
    }
    virtual void tick (int p) {
      pos = p;  
    }
    virtual void plot () {
      if (canvas==NULL) return;
      canvas->clearPixels();

      uint32_t color = CANVAS::Color(0,((pos%2==0)?255:128),0);
      if (*pause) color = CANVAS::Color(0,64,0);

      canvas->set(1,color);
      canvas->set(9,color);
      canvas->set(10,color);
      canvas->set(17,color);

      for (int i=0; i<16; i++) {
        int r = i/4;
        int c = 4+i%4;

        color = seq->scheme.low;
        if (i==seq->channel) color = seq->scheme.secondary;

        canvas->set(c+r*8,color);
      }
    }
    void setPause (bool* b) {
      pause = b;  
    }
    void setSeq (SEQ* s) {
      seq = s;  
    }
};
