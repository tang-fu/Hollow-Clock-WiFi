#define STEPS_PER_ROTATION 30743  // steps for a full turn of minute rotor
int delaytime = 1;
// 改变转动方向使用 {14, 15, 13, 15};
int port[4] = { 15, 13, 12, 14 };
// sequence of stepper motor control
int seq[8][4] = {
  { LOW, HIGH, HIGH, LOW },
  { LOW, LOW, HIGH, LOW },
  { LOW, LOW, HIGH, HIGH },
  { LOW, LOW, LOW, HIGH },
  { HIGH, LOW, LOW, HIGH },
  { HIGH, LOW, LOW, LOW },
  { HIGH, HIGH, LOW, LOW },
  { LOW, HIGH, LOW, LOW }
};
boolean time_modify = true;
int count = 1;

void rotate(int step) {
  static int phase = 0;
  int i, j;
  int delta = (step > 0) ? 1 : 7;
  int dt = 20;

  step = (step > 0) ? step : -step;
  for (j = 0; j < step; j++) {
    phase = (phase + delta) % 8;
    for (i = 0; i < 4; i++) {
      digitalWrite(port[i], seq[phase][i]);
    }
    delay(dt);
    if (dt > delaytime) dt--;
  }
  // power cut
  for (i = 0; i < 4; i++) {
    digitalWrite(port[i], LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(port[0], OUTPUT);
  pinMode(port[1], OUTPUT);
  pinMode(port[2], OUTPUT);
  pinMode(port[3], OUTPUT);
}

void loop() {
  static long prev_min = 0, prev_pos = 0;
  long min;
  static long pos;

  if (time_modify) {
    Serial.print(count);
    Serial.println("次校准");
    pos = (STEPS_PER_ROTATION * 60) / 60;
    rotate(-20);  // for approach run
    rotate(20);   // approach run without heavy load
    if (pos - prev_pos > 0) {
      rotate(pos - prev_pos);
    }
    prev_pos = pos;
    delay(1000);
    time_modify = false;
  } 
  else {
    if (count > 11) {
      count = 12;
    } else {
      count++;
      time_modify = true;
      prev_pos = 0;
    }
  }
}
