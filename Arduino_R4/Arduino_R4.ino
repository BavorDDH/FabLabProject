#include "src/Examples/EPD_Test.h"
#include "src/e-Paper/EPD_4in2_V2.h"
#include <string.h>
#include "RTC.h"
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <time.h>

const char* ssid = "JICfree";
const char* password = "welcome2jic";

bool toTown = true;

unsigned long baseUnixTime; // seconds
unsigned long baseMillis; // milliseconds at sync moment

UBYTE* ScreenImage;

void setup() {
  if (DEV_Module_Init() != 0) {
    return;
  }

  initDisplay();
  initWifi();

  EPD_4IN2_V2_Clear();

  updateTopBar(0, 0, 0, toTown);
  updateDeparture("31 8:50 15 min", 0);
  updateDeparture("77 8:55 20 min", 1);
  updateDeparture("bla bla", 2);
}

unsigned long lastUpdate = 0;

void loop() {
  unsigned long now = millis();

  if (now - lastUpdate >= 1000) {
    lastUpdate += 1000;   // prevents cumulative drift

    unsigned long unixTime = currentUnixTime();

    int hours   = (unixTime % 86400L) / 3600;
    int minutes = (unixTime % 3600) / 60;
    int seconds = unixTime % 60;

    updateTopBar(hours, minutes, seconds, toTown);
  }
}