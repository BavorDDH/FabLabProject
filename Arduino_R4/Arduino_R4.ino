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
  initTime();
}

void loop() {
  RTCTime currentTime;
  RTC.getTime(currentTime);  // Read from internal hardware clock

  Paint_Clear(WHITE);
  Paint_DrawString_EN(0, 0, String(currentTime.getSeconds()).c_str(), &Font16, BLACK, WHITE);
  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, 400, 50);

  delay(500);

  return;
  /*bool success = timeClient.forceUpdate();  // force fetch if first call fails
  if (!success){
    Paint_Clear(WHITE);
    Paint_DrawString_EN(0, 0, WiFi.localIP().toString().c_str(), &Font16, BLACK, WHITE);
    //Paint_DrawTime(0, 0, &sPaint_time, &Font20, WHITE, BLACK);
    EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, 400, 50);
    return;
  }

  unsigned long epochTime = timeClient.getEpochTime();

  struct tm* ptm = gmtime((time_t*)&epochTime);  // now gives local time

  int hour = ptm->tm_hour;   // hours
  int minute = ptm->tm_min;  // minutes
  int second = ptm->tm_sec;  // seconds

  PAINT_TIME sPaint_time;
  sPaint_time.Hour = hour;
  sPaint_time.Min = minute;
  sPaint_time.Sec = second;
  for (;;) {
    sPaint_time.Sec = sPaint_time.Sec + 1;
    if (sPaint_time.Sec == 60) {
      timeClient.update();

      sPaint_time.Min = sPaint_time.Min + 1;
      sPaint_time.Sec = 0;
      if (sPaint_time.Min == 60) {
        sPaint_time.Hour = sPaint_time.Hour + 1;
        sPaint_time.Min = 0;
        if (sPaint_time.Hour == 24) {
          sPaint_time.Hour = 0;
          sPaint_time.Min = 0;
          sPaint_time.Sec = 0;
        }
      }
    }
    Paint_Clear(WHITE);
    Paint_DrawString_EN(0, 0, String(epochTime).c_str(), &Font16, BLACK, WHITE);
    //Paint_DrawTime(0, 0, &sPaint_time, &Font20, WHITE, BLACK);
    EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, 400, 50);
    DEV_Delay_ms(500);
  }

  delay(2000);*/
}

void initWifi() {
  WiFi.begin(ssid, password);

  Paint_Clear(WHITE);
  Paint_NewImage(ScreenImage, 400, 50, 0, WHITE);
  Paint_DrawString_EN(10, 0, "Connecting to wifi...", &Font16, BLACK, WHITE);
  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, 400, 50);

  // todo: fail safe?
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Paint_Clear(WHITE);
  Paint_DrawString_EN(10, 0, "Connected to wifi", &Font16, BLACK, WHITE);
  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, 400, 50);

  IPAddress res;
  // This returns 1 if it successfully finds the IP, 0 if it fails
  if (WiFi.hostByName("google.com", res)) {
    Paint_Clear(WHITE);
    Paint_DrawString_EN(10, 0, "Connected", &Font16, BLACK, WHITE);
    EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, 400, 50);
  } else {
    Paint_Clear(WHITE);
    Paint_DrawString_EN(10, 0, "Nope", &Font16, BLACK, WHITE);
    EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, 400, 50);
  }

  delay(5000);
}

void initTime() {
  Paint_Clear(WHITE);
  Paint_DrawString_EN(10, 0, "Getting time...", &Font16, BLACK, WHITE);
  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, 400, 50);

  RTC.begin();
  Udp.begin(localPort);

  unsigned long epoch = 0;
  int attempts = 0;

  // Keep trying for up to 10 seconds to get a valid NTP response
  while (epoch == 0 && attempts < 10) {
    Debug("Attempting NTP sync...");
    epoch = getNTPTime();
    if (epoch == 0) {
      delay(1000);
      attempts++;
    }
  }

  if (epoch > 0) {
    int timeZoneOffset = 2 * 3600;
    RTCTime startTime(epoch + timeZoneOffset);
    RTC.setTime(startTime);
    Paint_Clear(WHITE);
    Paint_DrawString_EN(10, 0, "Updated time", &Font16, BLACK, WHITE);
    EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, 400, 50);
  } else {
    Paint_Clear(WHITE);
    Paint_DrawString_EN(10, 0, "Failed to get time", &Font16, BLACK, WHITE);
    EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, 400, 50);
  }
}

void initDisplay() {
  EPD_4IN2_V2_Init();
  EPD_4IN2_V2_Clear();
  DEV_Delay_ms(500);

  // Create a new image cache
  /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
  UWORD Imagesize = ((EPD_4IN2_V2_WIDTH % 8 == 0) ? (EPD_4IN2_V2_WIDTH / 8) : (EPD_4IN2_V2_WIDTH / 8 + 1)) * EPD_4IN2_V2_HEIGHT;
  if ((ScreenImage = (UBYTE*)malloc(Imagesize)) == NULL) {
    Debug("Failed to apply for black memory...\r\n");
    return;
  }

  Paint_NewImage(ScreenImage, EPD_4IN2_V2_WIDTH, EPD_4IN2_V2_HEIGHT, 0, WHITE);

  Paint_SelectImage(ScreenImage);
}