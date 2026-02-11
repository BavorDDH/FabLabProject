sFONT* TopBarFont = &Font24;
const int TopBarMargin = 8;
const int TopBarHeight = TopBarMargin + TopBarFont->Height;

sFONT* ItemFont = &Font20;
const int ItemMargin = 8;
const int ItemHeight = ItemMargin + TopBarFont->Height;

void updateTopBar(int hours, int minutes, int seconds, bool toTown) {
  Paint_NewImage(ScreenImage, EPD_4IN2_V2_WIDTH, TopBarHeight, 0, WHITE);
  Paint_Clear(BLACK);

  char timeStr[9];  // "HH:MM:SS" + null terminator

  sprintf(timeStr, "%02d:%02d:%02d", hours, minutes, seconds);
  Paint_DrawString_EN(TopBarMargin, TopBarMargin, timeStr, TopBarFont, WHITE, BLACK);

  const char* dirrectionText;
  if (toTown) {
    dirrectionText = "Do mesta";
  } else {
    dirrectionText = "Z mesta";
  }

  uint16_t dirrectionTextWidth = strlen(dirrectionText) * TopBarFont->Width;

  Paint_DrawString_EN(EPD_4IN2_V2_WIDTH - dirrectionTextWidth - TopBarMargin, TopBarMargin, dirrectionText, TopBarFont, WHITE, BLACK);

  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, EPD_4IN2_V2_WIDTH, TopBarHeight);
  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, EPD_4IN2_V2_WIDTH, TopBarHeight);
}

void updateDeparture(const char* text, int index){
  Paint_NewImage(ScreenImage, EPD_4IN2_V2_WIDTH, ItemHeight, 0, WHITE);
  Paint_Clear(WHITE);

  int yPos = TopBarHeight + index * ItemHeight;

  Paint_DrawString_EN(ItemMargin, ItemMargin, text, ItemFont, BLACK, WHITE);

  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, yPos, EPD_4IN2_V2_WIDTH, yPos + ItemHeight);
  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, yPos, EPD_4IN2_V2_WIDTH, yPos + ItemHeight);
}