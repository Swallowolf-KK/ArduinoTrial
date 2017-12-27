#include <WiFiUdp.h>
#include <NTP.h>
#include <Time.h>
#include <TimeLib.h>
#include<ESP8266WiFi.h>

extern "C"{
#include "user_interface.h"
}
#define PIN_EN 13
#define SSID "Buffalo-G-05AE"
#define PASS "rckva8rrve8nv"
#define SENDTO "192.168.11.255"
#define PORT 1024
#define SLEEP_P 290*1000000
#define DEVICE "temp._1,"
#define TEMP_OFFSET -50.0

void setup() {
  int waiting=0;
  pinMode(PIN_EN, OUTPUT);
  Serial.begin(9600);
  Serial.println("温度/時間 表示ツール");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  while(WiFi.status() != WL_CONNECTED){
    delay(100);
    waiting++;
    digitalWrite(PIN_EN, waiting%2);
    if(waiting%10==0) Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  ntp_begin(2390);
  setTimeServer("s2csntp.miz.nao.ac.jp");
}

void loop() {
  WiFiUDP udp;
  udp.println("Connected.");
    time();
    Thermo();
  }
}

void time() {
  WiFiUDP udp;
  time_t n = now();
  time_t t;

  char s[20];
  const char* format = "%04d-%02d-%02d %02d:%02d:%02d";

  // 日本時間
  t = localtime(n, 9);
  sprintf(s, format, year(t), month(t), day(t), hour(t), minute(t), second(t));
  Serial.print("現在の時刻は(JST) :");
  Serial.println(s);

  // UTC
  t = localtime(n, 0);
  sprintf(s, format, year(t), month(t), day(t), hour(t), minute(t), second(t));
  Serial.print("UTC時刻は :");
  Serial.println(s);
  udp.beginPacket(SENDTO, PORT);
  udp.print("UTC時刻は :");
  udp.println(s);
  udp.endPacket();
  
}

void Thermo() {
  WiFiUDP udp;
  float temp;
  digitalWrite(PIN_EN, HIGH);
  delay(100);
  temp=(float)system_adc_read();
  digitalWrite(PIN_EN, LOW);
  temp *= 1000. / 1023. / 10.;
  temp += TEMP_OFFSET;
  udp.beginPacket(SENDTO, PORT);
  udp.print(DEVICE);
  udp.println(temp, 1);
  Serial.println(temp, 1);
  udp.endPacket();
}


void sleep() {
    delay(200);
    ESP.deepSleep(SLEEP_P, WAKE_RF_DEFAULT);
    while(1){
      delay(100);
    }
}
