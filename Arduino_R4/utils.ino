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

void initWifi() {
  WiFi.begin(ssid, password);

  Paint_Clear(WHITE);
  Paint_NewImage(ScreenImage, 400, 60, 0, WHITE);
  Paint_DrawString_EN(10, 0, "Connecting to wifi...", &Font16, BLACK, WHITE);
  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, 400, 60);

  // todo: fail safe?
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Paint_Clear(WHITE);
  Paint_DrawString_EN(10, 0, "Connected to wifi", &Font16, BLACK, WHITE);
  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, 400, 60);
}

void syncTime(unsigned long unixTimeFromApi) {
  baseUnixTime = unixTimeFromApi;
  baseMillis = millis();
}

unsigned long currentUnixTime() {
  unsigned long elapsedSeconds = (millis() - baseMillis) / 1000;
  return baseUnixTime + elapsedSeconds;
}