#include "src/Examples/EPD_Test.h"
#include "src/e-Paper/EPD_4in2_V2.h"
#include <string.h>
#include "RTC.h"
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <time.h>

const char* ssid = "JICfree";
const char* password = "welcome2jic";

UBYTE* ScreenImage;

// NTP Settings
unsigned int localPort = 2390;
IPAddress timeServerIP;
const char* ntpServerName = "pool.ntp.org";
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
WiFiUDP Udp;

void setup() {
  if (DEV_Module_Init() != 0) {
    return;
  }

  initDisplay();
  initWifi();

  uddateTopBar(true);
}

void loop() {
 
}