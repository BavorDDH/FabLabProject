sFONT* TopBarFont = &Font24;
const int TopBarMargin = 8;
const int TopBarHeight = TopBarMargin + TopBarFont->Height;
const int TopBarTextWidth = TopBarFont->Width * 8 + TopBarMargin;

sFONT* ItemFont = &Font20;
const int ItemMargin = 8;
const int ItemHeight = ItemMargin + ItemFont->Height;

const char* UpdatingText = "Updating";
sFONT* UpdatingTextFont = &Font16;
const int UpdatingTextLength = strlen(UpdatingText);
const int UpdatingTextMargin = 5;
const int UpdatingTextHeight = UpdatingTextMargin * 2 + UpdatingTextFont->Height;
const int UpdatingTextWidth = UpdatingTextFont->Width * UpdatingTextLength + UpdatingTextMargin * 2;

#define IMG_MODE_TOPBAR 0
#define IMG_MODE_TOPBAR_TEXT 1
#define IMG_MODE_DEPARTURE_ITEM 2
#define IMG_MODE_UPDATING_TEXT 3

int lastImageMode = -1;

void prepareImage(int imageMode) {
  if (imageMode == lastImageMode) {
    return;
  }

  switch (imageMode) {
    case IMG_MODE_TOPBAR:
      Paint_NewImage(ScreenImage, EPD_4IN2_V2_WIDTH, TopBarHeight, 0, BLACK);
      break;
    case IMG_MODE_TOPBAR_TEXT:
      Paint_NewImage(ScreenImage, TopBarTextWidth, TopBarHeight, 0, BLACK);
      break;
    case IMG_MODE_DEPARTURE_ITEM:
      Paint_NewImage(ScreenImage, EPD_4IN2_V2_WIDTH, ItemHeight, 0, WHITE);
      break;
    case IMG_MODE_UPDATING_TEXT:
      Paint_NewImage(ScreenImage, UpdatingTextWidth, UpdatingTextHeight, 0, BLACK);
      break;
  }

  lastImageMode = imageMode;
}

void updateTopBar(int hours, int minutes, int seconds, bool toTown) {
  prepareImage(IMG_MODE_TOPBAR);
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

void updateTopBarTime(int hours, int minutes, int seconds) {
  prepareImage(IMG_MODE_TOPBAR_TEXT);
  Paint_Clear(BLACK);

  char timeStr[9];  // "HH:MM:SS" + null terminator

  sprintf(timeStr, "%02d:%02d:%02d", hours, minutes, seconds);
  Paint_DrawString_EN(TopBarMargin, TopBarMargin, timeStr, TopBarFont, WHITE, BLACK);

  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, TopBarTextWidth, TopBarHeight);
  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, TopBarTextWidth, TopBarHeight);
}

void updateTopBarDirection(bool toTown) {
  prepareImage(IMG_MODE_TOPBAR_TEXT);
  Paint_Clear(BLACK);

  const char* dirrectionText;
  if (toTown) {
    dirrectionText = "Do mesta";
  } else {
    dirrectionText = "Z mesta";
  }

  uint16_t dirrectionTextWidth = strlen(dirrectionText) * TopBarFont->Width;

  Paint_DrawString_EN(TopBarTextWidth - dirrectionTextWidth - TopBarMargin, TopBarMargin, dirrectionText, TopBarFont, WHITE, BLACK);

  EPD_4IN2_V2_PartialDisplay(ScreenImage, EPD_4IN2_V2_WIDTH - TopBarTextWidth, 0, EPD_4IN2_V2_WIDTH, TopBarHeight);
  EPD_4IN2_V2_PartialDisplay(ScreenImage, EPD_4IN2_V2_WIDTH - TopBarTextWidth, 0, EPD_4IN2_V2_WIDTH, TopBarHeight);
}

void updateDeparture(const char* text, int index) {
  prepareImage(IMG_MODE_DEPARTURE_ITEM);
  Paint_Clear(WHITE);

  int yPos = TopBarHeight + index * ItemHeight;

  Paint_DrawString_EN(ItemMargin, ItemMargin, text, ItemFont, BLACK, WHITE);

  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, yPos, EPD_4IN2_V2_WIDTH, yPos + ItemHeight);
  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, yPos, EPD_4IN2_V2_WIDTH, yPos + ItemHeight);
}

void showUpdatingText(bool show) {
  prepareImage(IMG_MODE_UPDATING_TEXT);

  if (show) {
    Paint_Clear(BLACK);

    Paint_DrawString_EN(UpdatingTextMargin, UpdatingTextMargin, UpdatingText, UpdatingTextFont, WHITE, BLACK);
  } else {
    Paint_Clear(WHITE);
  }

  EPD_4IN2_V2_PartialDisplay(ScreenImage, EPD_4IN2_V2_WIDTH - UpdatingTextWidth, EPD_4IN2_V2_HEIGHT - UpdatingTextHeight, EPD_4IN2_V2_WIDTH, EPD_4IN2_V2_HEIGHT);
  EPD_4IN2_V2_PartialDisplay(ScreenImage, EPD_4IN2_V2_WIDTH - UpdatingTextWidth, EPD_4IN2_V2_HEIGHT - UpdatingTextHeight, EPD_4IN2_V2_WIDTH, EPD_4IN2_V2_HEIGHT);
}