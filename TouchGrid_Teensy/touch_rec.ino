#define HWSERIAL Serial3

struct Touch_Event {
  int pin;
  double time;  
};

struct Event_Buffer {
  int max = 16;
  int size = 0;
  Touch_Event buffer [16];

  Touch_Event get (int index) {
    return buffer[index];  
  }

  double getTimeDifference (int index1, int index2) {
    return buffer[index1].time-buffer[index2].time;
  }

  void add (Touch_Event e) {
    if (size==max) return;
    buffer[size] = e;
    size++;
  }

  void remove (int index) {
    for (int i=index; i<size-1; i++) {
      buffer[i] = buffer[i+1];
    }
    size--;
  }

  int contains (Touch_Event e) {
    for (int i=0; i<size; i++) if (buffer[i].pin==e.pin) return i;
    return -1;
  }

  int contains (int pin) {
    for (int i=0; i<size; i++) if (buffer[i].pin==pin) return i;
    return -1;
  }

  void autoClean () {
    for (int i=size-1; i>=0; i--) if (millis()-buffer[i].time>=200) remove(i);  
  }
};

Event_Buffer events;

struct Touch_Area {
  int pin;
  char msg;
  float value, ref;
  bool state;

  static const int buffer_length = 64;
  float v_buffer [buffer_length];
  int writePos;
  
  void init () {
    value = touchRead(pin);
  }
  void setRef () {
    ref = value;
  }
  void read () {
    value = value*0.9+touchRead(pin)*0.1;

    v_buffer[writePos] = value;
    writePos = (writePos+1) % buffer_length;
    
    if (!state && getSlope()>10) {
      events.add({pin,millis()});
      Serial.println(msg);
      HWSERIAL.write(msg);
      state = true;
    }
    else if (state && getSlope()<-5) {
      state = false;  
    }
  }
  float getSlope() {
    return (v_buffer[(writePos+buffer_length-1)%buffer_length]-v_buffer[writePos]);
  }
  float getPeaks() {
    if (getValue()>20) return getSlope();
  }
  float getValue() {
    return value-ref;  
  }
  void print () {
    Serial.print(getValue());
  }
};

void gndPin (int p) {
  pinMode(p,OUTPUT);
  digitalWrite(p,LOW);  
}

Touch_Area t1 = {23,'t'};
//Touch_Area t2 = {22};
//Touch_Area b1 = {19,'b'};
//Touch_Area b2 = {18};
//Touch_Area l = {17};
//Touch_Area r = {16};
Touch_Area r1 = {15,'r'};
//Touch_Area r2 = {1};
Touch_Area l1 = {0,'l'};
//Touch_Area l2 = {};

static const int a_length = 3;
Touch_Area * areas [a_length] = {&t1,&r1,&l1};

elapsedMillis timer;
bool LEDstate;

void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);

  HWSERIAL.begin(9600);
  
  delay(100);

  gndPin(22);
  gndPin(19);
  gndPin(18);
  gndPin(17);
  gndPin(16);
  gndPin(1);
  
  for (int i = 0; i< a_length; i++) areas[i]->init();
}

int counter = 100;

void loop() {
  if (counter>-1) counter--;
  if (counter==0) {
    for (int i = 0; i< a_length; i++) areas[i]->setRef();
  }
  
  for (int i = 0; i< a_length; i++) areas[i]->read();
  
  /*for (int i = 0; i< a_length; i++) {
    areas[i]->print();
    Serial.print(",");
  }
  Serial.print(-5);
  Serial.print(",");
  Serial.print(-5);
  Serial.print(",");
  Serial.println(12);*/
  

  
  delayMicroseconds(1000);  
}
