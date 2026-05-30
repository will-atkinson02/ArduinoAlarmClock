#include "DisplayManager.h"
#include "Globals.h"

void initializeDisplay() 
{
  // Initialize the OLED display
  System_Init();
  Serial.print(F("OLED_Init()...\r\n"));
  OLED_2IN42_Init();
  delay(500);
  OLED_2IN42_Clear();

  // Allocate memory for the image buffer
  allocateBlackImageMemory();
  // Select and clear the image
  Paint_SelectImage(BlackImage);
  Paint_Clear(BLACK);
}

void updateUI()
{
  Paint_Clear(BLACK); 
  handleTextBlink();

  if (!menuMode)
  {
    drawDefaultUI();
    updateDisplay();
    return;
  }
  switch(selectorMode)
  {
    case SELECTOR_NONE:
      drawMenuOptions();
      break;
    case SELECTOR_TRACK:
      drawTrackSelector();
      break;
    case SELECTOR_ALARM:
      drawAlarmSelector();
      break;
    case SELECTOR_TIME:
      drawTimeSelector();
      break;
    case SELECTOR_DATE:
      drawDateSelector();
  }
  updateDisplay();
}

void updateDisplay()
{
  OLED_2IN42_Display(BlackImage);
}

void allocateBlackImageMemory()
{
  Imagesize = ((OLED_2IN42_WIDTH % 8 == 0) ? (OLED_2IN42_WIDTH / 8) : (OLED_2IN42_WIDTH / 8 + 1)) * OLED_2IN42_HEIGHT;
  BlackImage = (UBYTE *)malloc(Imagesize);

  if (BlackImage == NULL) 
  {
    Serial.print("Failed to apply for black memory...\r\n");
  }
  Serial.print("Paint_NewImage\r\n");
  Paint_NewImage(BlackImage, OLED_2IN42_WIDTH, OLED_2IN42_HEIGHT, 270, BLACK);
}

void drawDefaultUI()
{
  char buf[9];
  char vol[3];
  char buf3[8]; 
  char buf4[13]; 

  // Track
  int bellX = 9;
  int bellY = 4;
  drawBell(bellX, bellY);
  Paint_DrawChar(bellX + 11, bellY - 1, 45, &Font12, WHITE, BLACK);

  snprintf(buf3, sizeof(buf3), "%02d", alarmID);
  Paint_DrawString_EN(bellX + 20, bellY - 1, buf3, &Font12, BLACK, WHITE);

  // Volume
  Paint_DrawLine(48, 3, 48, 14, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  Paint_DrawChar(bellX + 44, bellY - 1, 'V', &Font12, WHITE, BLACK);
  Paint_DrawChar(bellX + 53, bellY - 1, 45, &Font12, WHITE, BLACK);
  snprintf(vol, sizeof(vol), "%d", volume);
  Paint_DrawString_EN(bellX + 61, bellY - 1, vol, &Font12, BLACK, WHITE);

  // Seperating line
  Paint_DrawLine(bellX - 1, bellY + 13, bellX + 84, bellY + 13, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

  // Date
  int dateX = 8;
  int dateY = 21;
  
  snprintf(buf4, sizeof(buf4), "%s %02d/%02d/%02d", dayStr.c_str(), dayInt, month, year - 2000);
  Paint_DrawString_EN(dateX, dateY, buf4, &Font12, WHITE, WHITE);

  // Sun/Moon icon
  int sunX = 110;
  int sunY = 17;
  drawDayAndNightIcons(sunX, sunY, hour);

  // Seperator 2
  Paint_DrawLine(dateX, dateY + 15, dateX + 110, dateY + 15, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

  // Timer
  int timerX = 6; 
  int timerY = 40;

  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hour, minutes, seconds);
  for (int i = 0; i < 8; i++) 
  {
      Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
      timerX += 14;
  }
}

void drawMenuOptions()
{
  const char* menuStrings[] = 
  {
    "MENU", 
    "Set alarm audio", 
    "Set alarm time", 
    "Set time", 
    "Set date"
  };

  UWORD yPos = 4;

  Paint_DrawString_EN(10, yPos, menuStrings[0], &Font12, BLACK, WHITE);

  Paint_DrawLine(8, yPos + 12, 120, yPos + 12, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  
  for (int i = 1; i < 5; i++) 
  {
      if (i == menuSelector) 
      {
        Paint_DrawRectangle(8, yPos + 3 + (i * 11), 120, yPos + (i * 11) + 13, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
        Paint_DrawString_EN(10, yPos + 2 + (i * 11), menuStrings[i], &Font12, WHITE, BLACK);
      } 
      else 
      {
        Paint_DrawString_EN(10, yPos + 2 + (i * 11), menuStrings[i], &Font12, BLACK, WHITE);
      }
  } 
}

void handleTextBlink()
{
  unsigned long currentMillis = millis();
  static bool isOnPhase = true;

  if (isOnPhase && currentMillis - previousMillis >= onDuration) {
    previousMillis = currentMillis;
    blinkState = false;
    isOnPhase = false;
  } else if (!isOnPhase && currentMillis - previousMillis >= offDuration) {
    previousMillis = currentMillis;
    blinkState = true;
    isOnPhase = true;
  }
}

void drawBell(int bellX, int bellY) {
 for (int i = bellX + 3; i < bellX + 7; i++) {
      Paint_DrawPoint(i, bellY + 1, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    }
    for (int i = bellX + 2; i < bellX + 8; i++) {
      for (int j = bellY + 2; j < bellY + 7; j++) {
        Paint_DrawPoint(i, j, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      }
    }
    for (int i = bellX + 1; i < bellX + 9; i++) {
      Paint_DrawPoint(i, bellY + 7, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    }
    for (int i = bellX + 4; i < bellX + 6; i++) {
      Paint_DrawPoint(i, bellY + 8, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    }
}

void drawDayAndNightIcons(int sunX, int sunY, int hour) {
  if (hour > 5 && hour < 21) {
      Paint_DrawCircle(sunX, sunY, 2, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
      // N
      Paint_DrawPoint(sunX, sunY - 4, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      Paint_DrawPoint(sunX, sunY - 5, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      // S
      Paint_DrawPoint(sunX, sunY + 4, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      Paint_DrawPoint(sunX, sunY + 5, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      // E
      Paint_DrawPoint(sunX - 4, sunY, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      Paint_DrawPoint(sunX - 5, sunY, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      // W
      Paint_DrawPoint(sunX + 4, sunY, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      Paint_DrawPoint(sunX + 5, sunY, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      // SE
      Paint_DrawPoint(sunX - 3, sunY - 3, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      Paint_DrawPoint(sunX - 4, sunY - 4, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      // SW
      Paint_DrawPoint(sunX + 3, sunY - 3, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      Paint_DrawPoint(sunX + 4, sunY - 4, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      // NE
      Paint_DrawPoint(sunX - 3, sunY + 3, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      Paint_DrawPoint(sunX - 4, sunY + 4, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      // NW
      Paint_DrawPoint(sunX + 3, sunY + 3, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      Paint_DrawPoint(sunX + 4, sunY + 4, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    } else {
      Paint_DrawCircle(sunX, sunY, 5, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
      Paint_DrawCircle(sunX - 2, sunY - 2, 4, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
      Paint_DrawPoint(sunX - 5, sunY + 1, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      Paint_DrawPoint(sunX + 1, sunY - 5, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      Paint_DrawPoint(sunX + 2, sunY, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      Paint_DrawPoint(sunX, sunY + 2, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    }

    // container
    Paint_DrawLine(sunX - 8, sunY - 8, sunX - 8, sunY + 8, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(sunX + 8, sunY - 8, sunX + 8, sunY + 8, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(sunX - 8, sunY + 8, sunX + 8, sunY + 8, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(sunX - 8, sunY - 8, sunX + 8, sunY - 8, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
}

void drawTrackSelector()
{
  Paint_Clear(BLACK);

  const char* trackTitle = "TRACKS";
  const char* trackString = "Track ";
  const int tracksToShow = numberOfTracks < 4 ? numberOfTracks : 4;
  UWORD yPos = 4;

  Paint_DrawString_EN(10, yPos, trackTitle, &Font12, BLACK, WHITE);
  Paint_DrawLine(8, yPos + 12, 120, yPos + 12, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  
  for (int i = 1; i <= tracksToShow; i++)
  { 
    char displayString[25];
    snprintf(displayString, sizeof(displayString), "%s%d", trackString, i + windowStart - 1);
    if (i + windowStart - 1 == trackSelected) 
    {
      Paint_DrawRectangle(8, yPos + 3 + (i * 11), 120, yPos + (i * 11) + 13, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
      
      if (!(allowBlink && blinkState)) 
      {
        Paint_DrawString_EN(10, yPos + 2 + (i * 11), displayString, &Font12, WHITE, BLACK);
      } 
    } 
    else
    {
      Paint_DrawString_EN(10, yPos + 2 + (i * 11), displayString, &Font12, BLACK, WHITE);
    }
  } 
}

void drawAlarmSelector()
{
  const char* on = "On";
  const char* off = "Off";
  char buf[9];
  int timerX = 6; 
  int timerY = 26;
  int switchX = 41;
  int switchY = 10;

  if (tempAlarmOn) 
  {
    Paint_DrawRectangle(switchX, switchY + 1, switchX + 21, switchY + 11, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    if (allowBlink && alarmSelectorMode == 1) 
    {
      if (blinkState) 
      {
        Paint_DrawString_EN(switchX + 3, switchY, on, &Font12, WHITE, BLACK);
      }
    } else 
    {
      Paint_DrawString_EN(switchX + 3, switchY, on, &Font12, WHITE, BLACK);
    }
    Paint_DrawString_EN(switchX + 22, switchY, off, &Font12, BLACK, WHITE);
  } 
  else 
  {
    Paint_DrawRectangle(switchX + 23, switchY + 1, switchX + 44, switchY + 11, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_EN(switchX + 3, switchY, on, &Font12, WHITE, WHITE);
    if (allowBlink && alarmSelectorMode == 1) 
    {
      if (blinkState) 
      {
        Paint_DrawString_EN(switchX + 22, switchY, off, &Font12, WHITE, BLACK);
      }
    } 
    else 
    {
      Paint_DrawString_EN(switchX + 22, switchY, off, &Font12, WHITE, BLACK);
    }
  }
  Paint_DrawRectangle(switchX, switchY, switchX + 45, switchY + 11, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawLine(switchX + 22, switchY + 1, switchX + 22, switchY + 11, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", tempAlarmHour, tempAlarmMinute, alarmSecond);
  for (int i = 0; i < 8; i++) 
  {
    if (i < 2) 
    {
      if (allowBlink && alarmSelectorMode == 2) 
      {
        if (blinkState) 
        {
          Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
        }
      }
      else 
      {
        Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
      }
    } 
    else if (i > 2 && i < 5) 
    {
      if (allowBlink && alarmSelectorMode == 3) 
      {
        if (blinkState) 
        {
          Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
        }
      } else 
      {
        Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
      }
    } 
    else 
    {
      Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
    }
    timerX += 14;
  }
}

void drawTimeSelector()
{
  char buf[9];
  int timerX = 6; 
  int timerY = 23;

  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hour, minutes, seconds);
  for (int i = 0; i < 8; i++) 
  {
      if (i < 2) 
      {
        if (allowBlink && timeSelectorMode == 1) 
        {
          if (blinkState) 
          {
            Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
          }
        } 
        else 
        {
          Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
        }
      } 
      else if (i > 2 && i < 5) 
      {
        if (allowBlink && timeSelectorMode == 2) 
        {
          if (blinkState)
          {
            Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
          }
        } 
        else 
        {
          Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
        }
      } 
      else if (i > 5 && i < 8) 
      {
        if (allowBlink && timeSelectorMode == 3) 
        {
          if (blinkState) 
          {
            Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
          }
        } 
        else 
        {
          Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
        }
      } 
      else 
      {
        Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
      }
      timerX += 14;
  }
}

void drawDateSelector()
{
  int dateX = 6;
  int dateY = 23;
  char date[9];
  snprintf(date, sizeof(date), "%02d/%02d/%02d", tempDay, tempMonth, tempYear - 2000);
  for (int i = 0; i < 8; i++) 
  {
    if (i < 2) 
    {
      if (allowBlink && dateSelectorMode == 1) 
      {
        if (blinkState) 
        {
          Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
        }
      } 
      else 
      {
        Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
      }
    } 
    else if (i > 2 && i < 5)
    {
      if (allowBlink && dateSelectorMode == 2) 
      {
        if (blinkState) 
        {
          Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
        }
      } 
      else 
      {
        Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
      }
    } 
    else if (i > 5 && i < 8)
    {
      if (allowBlink && dateSelectorMode == 3) 
      {
        if (blinkState) 
        {
          Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
        }
      } 
      else 
      {
        Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
      }
    } 
    else 
    {
      Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
    }
    dateX += 14;
  }
}