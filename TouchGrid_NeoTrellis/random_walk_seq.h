struct GRAPH_STEP {
  int pitch;
  int8_t nextID [8];
  int8_t next_length;

  int8_t getNext () {
    return nextID[random(next_length)];
  }

  int8_t getNext (int8_t last) {
    int i = random(next_length);
    int8_t next = nextID[i];
    if (next == last) return nextID[(i + 1) % next_length];
    return next;
  }
};

class RANDOM_WALK_SEQ : public SEQ {
    GRAPH_STEP steps [24] = {
      // RANDOM
      /*{-1,{1,9,8,0,0,0,0,0},3},
        {-1,{2,10,9,7,1,0,0,0},5},
        {-1,{3,11,10,9,2,0,0,0},5},
        {-1,{4,12,11,10,3,0,0,0},5},
        {-1,{5,13,12,11,4,0,0,0},5},
        {-1,{6,14,13,12,5,0,0,0},5},
        {-1,{7,15,14,13,6,0,0,0},5},
        {-1,{15,14,6,0,0,0,0,0},3},

        {-1,{0,1,9,17,16,0,0,0},5},
        {-1,{0,1,2,10,18,17,16,8},8},
        {-1,{1,2,3,11,19,18,17,9},8},
        {-1,{2,3,4,12,20,19,18,10},8},
        {-1,{3,4,5,13,21,20,19,11},8},
        {-1,{4,5,6,14,22,21,20,12},8},
        {-1,{5,6,7,15,23,22,21,13},8},
        {-1,{6,7,23,22,14,0,0,0},5},

        {-1,{8,9,17,0,0,0,0,0},3},
        {-1,{8,9,10,18,16,0,0,0},5},
        {-1,{9,10,11,19,17,0,0,0},5},
        {-1,{10,11,12,20,18,0,0,0},5},
        {-1,{11,12,13,21,19,0,0,0},5},
        {-1,{12,13,14,22,20,0,0,0},5},
        {-1,{13,14,15,23,21,0,0,0},5},
        {-1,{15,22,14,0,0,0,0,0},3}*/

      //
      { -1, {1, 9, 0, 0, 0, 0, 0, 0}, 2},
      { -1, {2, 10, 0, 0, 0, 0, 0, 0}, 2},
      { -1, {3, 11, 0, 0, 0, 0, 0, 0}, 2},
      { -1, {4, 12, 0, 0, 0, 0, 0, 0}, 2},
      { -1, {5, 13, 0, 0, 0, 0, 0, 0}, 2},
      { -1, {6, 14, 0, 0, 0, 0, 0, 0}, 2},
      { -1, {7, 0, 0, 0, 0, 0, 0, 0}, 1},
      { -1, {0, 0, 0, 0, 0, 0, 0, 0}, 1},

      { -1, {9, 0, 0, 0, 0, 0, 0, 0}, 1},
      { -1, {2, 10, 18, 0, 0, 0, 0, 0}, 3},
      { -1, {3, 11, 19, 0, 0, 0, 0, 0}, 3},
      { -1, {4, 12, 20, 0, 0, 0, 0, 0}, 3},
      { -1, {5, 13, 21, 0, 0, 0, 0, 0}, 3},
      { -1, {6, 14, 0, 0, 0, 0, 0, 0}, 2},
      { -1, {7, 0, 0, 0, 0, 0, 0, 0}, 1},
      { -1, {0, 0, 0, 0, 0, 0, 0, 0}, 1},

      { -1, {1, 0, 0, 0, 0, 0, 0, 0}, 1},
      { -1, {2, 0, 0, 0, 0, 0, 0, 0}, 1},
      { -1, {11, 19, 0, 0, 0, 0, 0, 0}, 2},
      { -1, {12, 20, 0, 0, 0, 0, 0, 0}, 2},
      { -1, {13, 21, 0, 0, 0, 0, 0, 0}, 2},
      { -1, {14, 0, 0, 0, 0, 0, 0, 0}, 1},
      { -1, {0, 0, 0, 0, 0, 0, 0, 0}, 1},
      { -1, {0, 0, 0, 0, 0, 0, 0, 0}, 1}
    };

    int8_t pos;
    int8_t last_pos;

    int held_key = -1;

    int active_note = -1;
    byte active_channel;

  public:
    void handle (keypadEvent e) {
      if (e.bit.EVENT == KEY_JUST_PRESSED) {
        if (held_key == -1 && e.bit.KEY >= 24) {
          held_key = (int8_t)e.bit.KEY;
        }
        else if (e.bit.KEY < 24) {
          if (steps[e.bit.KEY].pitch == held_key % 8) {
            steps[e.bit.KEY].pitch = -1;
          }
          else {
            steps[e.bit.KEY].pitch = held_key % 8;
          }
        }
      }
      if (e.bit.EVENT == KEY_JUST_RELEASED) {
        if (held_key == -1 && e.bit.KEY < 24) {
          steps[e.bit.KEY].pitch = -1;
        }
        if (held_key == e.bit.KEY) {
          held_key = -1;
        }

      }
    }

    void tick (int p) {
      last_pos = pos;
      pos = steps[pos].getNext(last_pos);

      // old Notes off
      if (active_note > -1) {
        noteOff(active_channel, active_note, 0);
        active_note = -1;
      }

      // new Notes on
      if (steps[pos].pitch > -1) {
        active_note = SCALE::get(steps[pos].pitch, scale);
        active_channel = channel;
        noteOn(active_channel, active_note, 100);
      }
    }

    uint32_t pitchToColor (int pitch) {
      if (pitch == -1) return CANVAS::Color(0, 0, 0);
      return CANVAS::Color((pitch * 32), 0, 255 - (pitch * 32));
    }

    uint32_t pitchToColor (int pitch, float intensity) {
      if (pitch == -1) return CANVAS::Color(0, 0, 0);
      return CANVAS::Color((pitch * 32) * intensity, 0, (255 - (pitch * 32)) * intensity);
    }

    void plot () {
      if (canvas == NULL) return;
      // KEYS
      for (int i = 0; i < 8; i++) canvas->set(24 + i, pitchToColor(i, ((active_note == SCALE::get(i, scale)) ? 1 : 0.66)));

      for (int i = 0; i < 24; i++) {
        // HEAD
        if (i == pos) canvas->set(pos, ((steps[i].pitch == -1) ? scheme.main : pitchToColor(steps[i].pitch)));
        // STEPS
        else canvas->set(i, pitchToColor(steps[i].pitch, 0.33));
      }
    }
};
