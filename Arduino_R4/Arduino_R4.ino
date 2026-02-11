#include "src/Examples/EPD_Test.h"
#include "src/e-Paper/EPD_4in2_V2.h"
#include <string.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

const int buttonPin = 2;
int lastButtonState = HIGH;
int buttonState;

#define MAX_DEPARTURES 5
#define MAX_DEPARTURE_STR_LEN 32
// must be larger than HTTP_TIMEOUT
#define API_UPDATE_FREQUENCY 60 * 1000
bool toTown = true;

struct DepartureResult {
  unsigned long long time;
  char departures[MAX_DEPARTURES][MAX_DEPARTURE_STR_LEN];
  int count;
  bool success;
};
DepartureResult currentData;

unsigned long long baseUnixTime;  // seconds
unsigned long baseMillis;    // milliseconds at sync moment

UBYTE* ScreenImage;

void setup() {
  if (DEV_Module_Init() != 0) {
    return;
  }

  initDisplay();
  initWifi();

  pinMode(buttonPin, INPUT_PULLUP);

  EPD_4IN2_V2_Clear();

  updateTopBar(0, 0, 0, toTown);

  startRequest(MAX_DEPARTURES, toTown);
}

unsigned long lastSecondTick = 0;
unsigned long lastApiTick = 0;

// todo: full clear every x minutes (EPD_4IN2_V2_Clear)
void loop() {
  unsigned long now = millis();

  // update button
  buttonState = digitalRead(buttonPin);

  if (lastButtonState == HIGH && buttonState == LOW) {
    toTown = !toTown;
    updateTopBarDirection(toTown);
    startRequest(MAX_DEPARTURES, toTown);
    lastApiTick = now;
  }

  lastButtonState = buttonState;

  // update every second
  if (now - lastSecondTick >= 1000) {
    lastSecondTick += 1000;

    unsigned long long unixTime = currentUnixTime();

    int hours = (unixTime % 86400UL) / 3600;
    int minutes = (unixTime % 3600UL) / 60;
    int seconds = unixTime % 60;

    updateTopBarTime(hours, minutes, seconds);
  }

  // get every minute (60s), must be larger than HTTP_TIMEOUT
  if (now - lastApiTick >= API_UPDATE_FREQUENCY) {
    lastApiTick += API_UPDATE_FREQUENCY;

    startRequest(MAX_DEPARTURES, toTown);
  }

  if (pollRequest(currentData) && currentData.success) {
    syncTime(currentData.time);

    // todo: one at a time
    for (int i = 0; i < currentData.count; i++) {
      updateDeparture(currentData.departures[i], i);
    }
  }
}