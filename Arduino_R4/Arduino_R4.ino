#include "src/Examples/EPD_Test.h"
#include "src/e-Paper/EPD_4in2_V2.h"
#include <string.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

const int buttonPin = 2;
int lastButtonState = HIGH;
int buttonState;

#define MAX_DEPARTURES 7
#define MAX_DEPARTURE_STR_LEN 48
// must be larger than HTTP_TIMEOUT
#define API_UPDATE_FREQUENCY 60 * 1000
bool toTown = true;
bool queueRequest = false;
int nextIndexToUpdate = 0;

struct DepartureResult {
  unsigned long long time;
  char departures[MAX_DEPARTURES][MAX_DEPARTURE_STR_LEN];
  int count;
  bool success;
};
DepartureResult currentData;

enum RequestState {
  REQUEST_NONE,
  REQUEST_HTTP,
  REQUEST_UI,
};
RequestState currentRequestState;

unsigned long long baseUnixTime;  // seconds
unsigned long baseMillis;         // milliseconds at sync moment

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
    fetchNewData(true, now);
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

    fetchNewData(false, now);
  }

  if (queueRequest && currentRequestState == REQUEST_UI){
    queueRequest = false;
    fetchNewData(true, now);
  }

  if (/*currentRequestState == REQUEST_HTTP && */pollRequest(currentData)) {
    if (currentData.success) {
      syncTime(currentData.time);
      setRequestState(REQUEST_UI);

      nextIndexToUpdate = 0;
    } else {
      setRequestState(REQUEST_NONE);
    }
  }

  if (currentRequestState == REQUEST_UI) {
    updateDeparture(currentData.departures[nextIndexToUpdate], nextIndexToUpdate);

    nextIndexToUpdate++;
    if (nextIndexToUpdate >= currentData.count) {
      setRequestState(REQUEST_NONE);
    }
  }
}

void fetchNewData(bool force, unsigned long now) {
  if (currentRequestState != REQUEST_NONE && (currentRequestState != REQUEST_UI || !force)) {
    if (force) {
      queueRequest = true;
    }

    return;
  }

  setRequestState(REQUEST_HTTP);
  startRequest(MAX_DEPARTURES, toTown);
  lastApiTick = now;
}

void setRequestState(RequestState newState) {
  bool lastShown = currentRequestState != REQUEST_NONE;
  bool newShown = newState != REQUEST_NONE;
  if (lastShown != newShown) {
    showUpdatingText(newShown);
  }

  //Serial.write("Set state to: ");
  //Serial.write(String(newState).c_str());
  currentRequestState = newState;
}