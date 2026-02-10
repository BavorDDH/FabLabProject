void uddateTopBar(bool toTown) {
  sFONT* Font = &Font24;
  const int Margin = 5;
  const int TopBarHeight = Margin + Font->Height;

  Paint_NewImage(ScreenImage, EPD_4IN2_V2_WIDTH, TopBarHeight, 0, WHITE);
  Paint_Clear(BLACK);

  Paint_DrawString_EN(Margin, Margin, "08:30", Font, WHITE, BLACK);

  const char* dirrectionText;
  if (toTown) {
    dirrectionText = "Do mesta";
  } else {
    dirrectionText = "Z mesta";
  }
  uint16_t dirrectionTextWidth = strlen(dirrectionText) * Font->Width;

  Paint_DrawString_EN(EPD_4IN2_V2_WIDTH - dirrectionTextWidth - Margin, Margin, dirrectionText, Font, WHITE, BLACK);

  EPD_4IN2_V2_PartialDisplay(ScreenImage, 0, 0, EPD_4IN2_V2_WIDTH, TopBarHeight);
}