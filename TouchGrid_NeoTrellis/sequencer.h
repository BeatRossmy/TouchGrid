struct COLOR_SCHEME {
  uint32_t main = CANVAS::Color(255, 0, 0);
  uint32_t secondary = CANVAS::Color(100, 0, 0);
  uint32_t highlight = CANVAS::Color(255, 100, 0);
  uint32_t low = CANVAS::Color(255, 100, 0);
};

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

enum SCALE_TYPE {CHROMATIC,MINOR,MAJOR,PENTATONIC,DRUMS};
const int chrm [8] = {60,61,62,63,64,65,66,67};
const int minr [8] = {60,62,63,65,67,68,70,72};
const int majr [8] = {60,62,64,65,67,69,71,72};
const int pent [8] = {60,62,65,67,69,72,74,77};
const int drum [8] = {36,37,38,39,40,41,42,43};

class SCALE {
  public:
  static int get (int step, SCALE_TYPE scale) {
    int pitch = 0;
    if (step<0 || step>=8) return pitch;
    if (scale==CHROMATIC) pitch = chrm[step%8];
    if (scale==MINOR) pitch = minr[step%8];
    if (scale==MAJOR) pitch = majr[step%8];
    if (scale==PENTATONIC) pitch = pent[step%8];
    if (scale==DRUMS) pitch = drum[step%8];
    return pitch;
  }  
};

class SEQ {
  public:
    COLOR_SCHEME scheme;
    CANVAS* canvas = NULL;
    SCALE_TYPE scale = PENTATONIC;
    byte channel = 0;
    void setColors (uint32_t m, uint32_t s, uint32_t h, uint32_t l) {
      scheme.main = m;
      scheme.secondary = s;
      scheme.highlight = h;
      scheme.low = l;
    }
    void setChannel (byte c) {
      channel = c;
    } 
    //void init () {}
    void setCanvas (CANVAS* c) {canvas = c;}
    virtual void handle (keypadEvent e) = 0;
    virtual void tick (int pos) = 0;
    virtual void plot () = 0;
};
