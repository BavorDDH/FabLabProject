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

void setup() {
  if (DEV_Module_Init() != 0) {
    return;
  }

  initDisplay();
  initWifi();

  EPD_4IN2_V2_Clear();

  uddateTopBar(0, 0, 0, true);
  updateDeparture("31 8:50 15 min", 0);
  updateDeparture("77 8:55 20 min", 1);
  updateDeparture("bla bla", 2);
}

int hours = 8;
int minutes = 30;
int seconds = 15;

void loop() {
  seconds = seconds + 1;
  if (seconds == 60) {
    minutes = minutes + 1;
    seconds = 0;
    if (minutes == 60) {
      hours = hours + 1;
      minutes = 0;
      if (hours == 24) {
        hours = 0;
        minutes = 0;
        seconds = 0;
      }
    }
  }

  //uddateTopBar(hours, minutes, seconds, true);

  DEV_Delay_ms(500);
}