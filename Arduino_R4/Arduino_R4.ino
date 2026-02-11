#include "src/Examples/EPD_Test.h"
#include "src/e-Paper/EPD_4in2_V2.h"
#include <string.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

const char* ssid = "JICfree";
const char* password = "welcome2jic";

#define MAX_DEPARTURES 5
#define MAX_DEPARTURE_STR_LEN 32
bool toTown = true;

struct DepartureResult {
  unsigned long time;
  char departures[MAX_DEPARTURES][MAX_DEPARTURE_STR_LEN];
  int count;
  bool success;
};
DepartureResult currentData;

unsigned long baseUnixTime;  // seconds
unsigned long baseMillis;    // milliseconds at sync moment

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

  startRequest(MAX_DEPARTURES);
}

unsigned long lastSecondTick = 0;
unsigned long lastApiTick = 0;

void loop() {
  unsigned long now = millis();

  // update every second
  if (now - lastSecondTick >= 1000) {
    lastSecondTick += 1000;

    unsigned long unixTime = currentUnixTime();

    int hours = (unixTime % 86400UL) / 3600;
    int minutes = (unixTime % 3600UL) / 60;
    int seconds = unixTime % 60;

    updateTopBar(hours, minutes, seconds, toTown);
  }

  // get every minute (60s), must be larger than HTTP_TIMEOUT
  if (now - lastApiTick >= 60000) {
    lastApiTick += 60000;

    startRequest(MAX_DEPARTURES);
  }

  if (pollRequest(currentData) && currentData.success) {
    syncTime(currentData.time);

    // todo: one at a time
    for (int i = 0; i < currentData.count; i++) {
      updateDeparture(currentData.departures[i], i);
    }
  }
}