#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
// 4096 * 90 / 12 = 30720 default
#define STEPS_PER_ROTATION 30743
const char *ssid      = "INPUT_YOUR_WIFI_ID";
const char *password  = "INPUT_YOUR_WIFI_PASSWORD";
//int currentHour = 0;
int currentMinute = 0;
//int currentSeconds = 0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com", 60 * 60 * 8, 30 * 60 * 1000);  //偏移
int delaytime = 1;
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

void setup() {
  Serial.begin(115200);
  WiFi_Begin();
  timeClient.begin();
  pinMode(port[0], OUTPUT);
  pinMode(port[1], OUTPUT);
  pinMode(port[2], OUTPUT);
  pinMode(port[3], OUTPUT);
  rotate(-20);  // for approach run
  rotate(20);   // approach run without heavy load
  //rotate(STEPS_PER_ROTATION / 60);
}

void loop() {
  static long prev_min = 0, prev_pos = 0;
  long min;
  static long pos;

  min = Get_Current_Time();

  if (prev_min == min) {
    return;
  }

  prev_min = min;
  pos = (STEPS_PER_ROTATION * min) / 60;
  rotate(-20);  // for approach run
  rotate(20);   // approach run without heavy load
  if (pos - prev_pos > 0) {
    rotate(pos - prev_pos);
  }
  prev_pos = pos;
}

void WiFi_Begin() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi connected!");
}

int Get_Current_Time() {
  timeClient.update();
  //currentHour = timeClient.getHours();
  currentMinute = timeClient.getMinutes();
  //currentSeconds = timeClient.getSeconds();
  //Serial.println(currentMinute);//串口检查时间
  if (currentMinute == 0) {
    return 60;
  } else {
    return currentMinute;
  }
}

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