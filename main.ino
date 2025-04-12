// DFPlayer
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Display
#include "OLED_Driver.h"
#include "GUI_paint.h"
#include "DEV_Config.h"
#include "Debug.h"
#include "ImageData.h"
bool screenOn = true;

// Display globals
UBYTE *BlackImage;
UWORD Imagesize;

// DFPlayer globals
DFRobotDFPlayerMini myDFPlayer;
String line;
char command;
int volume = 10;
int pause = 0;
int repeat = 0;
int alarmID = 1;
int numberOfTracks;

// RTC
#include <Wire.h>
#include <RTClib.h>
#include <RTC.h>
RTC_DS3231 rtc;

int year;
int month;
int dayInt;
String dayStr;
int hour;
int minutes;
int seconds;

// alarm settings
bool alarmPlaying = false;
bool alarmOn = false;
int alarmHour = 9;
int alarmMinute = 1;
int alarmSecond = 0;

//buttons
const int plusPin = 2;
const int minusPin = 3;
const int selectPin = 4;
const int menuPin = 5;
bool selectStatePrevious = HIGH;
bool plusStatePrevious = HIGH;
bool minusStatePrevious = HIGH;
bool menuStatePrevious = HIGH;

// menu
bool menuMode = false;
int menuSelector;
int selectorMode = -1;

// track selector
int trackSelected = -1;
bool testTrackPlaying = false;
int windowStart = 1;
int windowEnd = 4;

// alarm selector
int alarmSelectorMode;
bool tempAlarmOn;
int tempAlarmHour;
int tempAlarmMinute;

// time selector
int timeSelectorMode;
int tempHour;
int tempMinutes;
int tempSeconds;

// date selector
int dateSelectorMode;
int tempDay;
int tempMonth;
int tempYear;

// blink animation
bool allowBlink = false;
unsigned long previousMillis = 0;
const long onDuration = 600;
const long offDuration = 500;
bool blinkState = true;

#define MAX_VOLUME 20
#define MIN_VOLUME 0

void setup() {
  initializePins();
  initializeSerialCommunication();
  initializeDFPlayer();
  initializeDisplay();
  initializeRTC();
}

void loop() {
  DateTime now = rtc.now(); // Get current time

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  
  //Time handling
  RTCTime currentTime;
  RTC.getTime(currentTime);

  Serial.println("hello 2");

  if (selectorMode == 3) {
    hour = tempHour;
    minutes = tempMinutes;
    seconds = tempSeconds;
  } else {
    hour = currentTime.getHour();
    minutes = currentTime.getMinutes();
    seconds = currentTime.getSeconds();
  }
  year = currentTime.getYear();
  month = Month2int(currentTime.getMonth());
  dayInt = currentTime.getDayOfMonth();
  dayStr = dayToString((int)currentTime.getDayOfWeek());
  // activate alarm
  if (alarmOn && !menuMode && alarmHour == hour && alarmMinute == minutes && alarmSecond == seconds) {
    myDFPlayer.play(alarmID);
    alarmPlaying = true;
    if (!screenOn) {
      screenOn = true;
      turnOnScreen();
    }
  }

  // select button
  bool selectState = digitalRead(selectPin);
  if (selectState == LOW && selectStatePrevious == HIGH) {
    if (!alarmPlaying && !menuMode && screenOn) {
      screenOn = false;
      turnOffScreen();
    } else if (!alarmPlaying && !menuMode && !screenOn) {
      screenOn = true;
      turnOnScreen();
    }

    if (alarmPlaying) {
      alarmPlaying = false;
      myDFPlayer.stop();
    } else if (menuMode) {
      if (selectorMode == -1) { //choosing a selectorMode
        selectorMode = menuSelector;
        if (selectorMode == 1) {
          trackSelected = 1;
        } else if (selectorMode == 2) {
          alarmSelectorMode = 1;
          allowBlink = true;
          // create temp variables to store changes
          tempAlarmOn = alarmOn;
          tempAlarmHour = alarmHour;
          tempAlarmMinute = alarmMinute;
        } else if (selectorMode == 3) {
          timeSelectorMode = 1;
          allowBlink = true;
          // create temp variables to store changes
          tempHour = hour;
          tempMinutes = minutes;
          tempSeconds = seconds;
        } else if (selectorMode == 4) {
          dateSelectorMode = 1;
          allowBlink = true;
          // create temp variables to store changes
          tempDay = dayInt;
          tempMonth = month;
          tempYear = year;
        }
      } else if (selectorMode == 1) { //navigating each respective selectorModes
        if (testTrackPlaying) {
          // confirm changes and return to home screen
          alarmID = trackSelected;
          myDFPlayer.stop();
          menuMode = false;
          selectorMode = -1;
          trackSelected = -1;
          testTrackPlaying = false;
          allowBlink = false;
        } else {
          testTrackPlaying = true;
          allowBlink = true;
          myDFPlayer.play(trackSelected);
        }
      } else if (selectorMode == 2) {
        if (alarmSelectorMode < 3) {
          alarmSelectorMode++;
        } else {
          alarmOn = tempAlarmOn;
          alarmHour = tempAlarmHour;
          alarmMinute = tempAlarmMinute;
          selectorMode = -1;
          allowBlink = false;
        }
      } else if (selectorMode == 3) {
        if (timeSelectorMode < 3) {
          timeSelectorMode++;
        } else {
          selectorMode = -1;
          allowBlink = false;
          RTCTime startTime(currentTime.getDayOfMonth(), currentTime.getMonth(), currentTime.getYear(), tempHour, tempMinutes, tempSeconds, currentTime.getDayOfWeek(), SaveLight::SAVING_TIME_ACTIVE);
          RTC.setTime(startTime);
        }
      } else if (selectorMode == 4) {
        if (dateSelectorMode < 3) {
          dateSelectorMode++;
        } else {
          selectorMode = -1;
          allowBlink = false;
          // Think about day of week
          RTCTime startTime(tempDay, static_cast<Month>(tempMonth), tempYear, currentTime.getHour(), currentTime.getMinutes(), currentTime.getSeconds(), currentTime.getDayOfWeek(), SaveLight::SAVING_TIME_ACTIVE);
          RTC.setTime(startTime);
        }
      }
    }
  }
  selectStatePrevious = selectState;

  // plus
  bool plusState = digitalRead(plusPin);
  if (plusState == LOW && plusStatePrevious == HIGH) {
    if (menuMode) {
      if (selectorMode == -1) {
        if (menuSelector == 4) {
          menuSelector = 1;
        } else {
          menuSelector++;
        }
      } else if (selectorMode == 1) {
        if (testTrackPlaying) {
          if (volume < MAX_VOLUME) {
            myDFPlayer.volumeUp();
          }
        } else {
          if (trackSelected == numberOfTracks) {
            windowStart = 1;
            windowEnd = 4;
            trackSelected = 1;
          } else if (trackSelected == windowEnd && trackSelected < numberOfTracks) {
            windowStart++;
            windowEnd++;
            trackSelected++;
          } else {
            trackSelected++;
          }
        }
      } else if (selectorMode == 2) { // set alarm time
        if (alarmSelectorMode == 1) {
          tempAlarmOn = !tempAlarmOn;
        } else if (alarmSelectorMode == 2) {
          if (tempAlarmHour == 23) {
            tempAlarmHour == 0;
          } else {
            tempAlarmHour++;
          }
        } else {
          if (tempAlarmMinute == 59) {
            tempAlarmMinute = 0;
          } else {
            tempAlarmMinute++;
          }
        }
      } else if (selectorMode == 3) { // set time
        if (timeSelectorMode == 1) {
          if (tempHour == 23) {
            tempHour = 0;
          } else {
            tempHour++;
          }
        } else if (timeSelectorMode == 2) {
          if (tempMinutes == 59) {
            tempMinutes = 0;
          } else {
            tempMinutes++;
          }
        } else {
          if (tempSeconds == 59) {
            tempSeconds = 0;
          } else {
            tempSeconds++;
          }
        }
      } else if (selectorMode == 4) {
        if (dateSelectorMode == 1) {
          //configure for months and leap year
          if (tempDay == 30) {
            tempDay = 0;
          } else {
            tempDay++;
          }
        } else if (dateSelectorMode == 2) {
          if (tempMonth == 12) {
            tempMonth = 0;
          } else {
            tempMonth++;
          }
        } else if (dateSelectorMode == 3) {
          if (tempYear == 2099) {
            tempYear = 2000;
          } else {
            tempYear++;
          }
        }
      }
    } else if (volume < MAX_VOLUME) {
      myDFPlayer.volumeUp();
      volume++;
    }
  }
  plusStatePrevious = plusState;
  
  // minus
  bool minusState = digitalRead(minusPin);
  if (minusState == LOW && minusStatePrevious == HIGH) {
    if (menuMode) {
      if (selectorMode == -1) {
        if (menuSelector == 1) {
          menuSelector = 4;
        } else {
          menuSelector--;
        }
      } else if (selectorMode == 1) { // select alarm audio
        if (testTrackPlaying) {
          if (volume > MIN_VOLUME) {
            myDFPlayer.volumeDown();
          }
        } else {
          if (trackSelected == 1) {
            windowStart = numberOfTracks - 3;
            windowEnd = numberOfTracks;
            trackSelected = numberOfTracks;
          } else if (trackSelected == windowStart && windowStart > 1) {
            windowStart--;
            windowEnd--;
            trackSelected--;
          } else {
            trackSelected--;
          }
        }
      } else if (selectorMode == 2) { // set alarm time
        if (alarmSelectorMode == 1) {
          tempAlarmOn = !tempAlarmOn;
        } else if (alarmSelectorMode == 2) {
          if (tempAlarmHour == 0) {
            tempAlarmHour = 23;
          } else {
            tempAlarmHour--;
          }
        } else {
          if (tempAlarmMinute == 0) {
            tempAlarmMinute = 59;
          } else {
            tempAlarmMinute--;
          }
        }
      } else if (selectorMode == 3) {
        if (timeSelectorMode == 1) {
          if (tempHour == 0) {
            tempHour = 23;
          } else {
            tempHour--;
          }
        } else if (timeSelectorMode == 2) {
          if (tempMinutes == 0) {
            tempMinutes = 59;
          } else {
            tempMinutes--;
          }
        } else {
          if (tempSeconds == 0) {
            tempSeconds = 59;
          } else {
            tempSeconds--;
          }
        }
      } else if (selectorMode == 4) {
        if (dateSelectorMode == 1) {
          //configure for months and leap year
          if (tempDay == 0) {
            tempDay = 30;
          } else {
            tempDay--;
          }
        } else if (dateSelectorMode == 2) {
          if (tempMonth == 0) {
            tempMonth = 12;
          } else {
            tempMonth--;
          }
        } else if (dateSelectorMode == 3) {
          if (tempYear - 2000 == 0) {
            tempYear = 2099;
          } else {
            tempYear--;
          }
        }
      }
    } else if (volume > MIN_VOLUME) {
      myDFPlayer.volumeDown();
      volume--;
    }
  }
  minusStatePrevious = minusState;

  // menu button
  bool menuState = digitalRead(menuPin);
  if (menuState == LOW && menuStatePrevious == HIGH) {
    if (selectorMode == -1) {
      menuMode = !menuMode;
      menuSelector = 1; 
    } else if (selectorMode == 1) {
      if (testTrackPlaying) {
        myDFPlayer.stop();
        allowBlink = false;
        testTrackPlaying = false;
      } else {
        selectorMode = -1;
      }
    } else if (selectorMode == 2) {
      if (alarmSelectorMode > 1) {
        alarmSelectorMode--;
      } else {
        selectorMode = -1;
        allowBlink == false;
      }
    } else if (selectorMode == 3) {
      selectorMode = -1;
      allowBlink = false;
    } else if (selectorMode == 4) {
      if (dateSelectorMode > 1) {
        dateSelectorMode--;
      } else {
        selectorMode = -1;
        allowBlink = false;
      }
    }
  }

  menuStatePrevious = menuState;
  
  Paint_Clear(BLACK);
  
  // text blink
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


  // Open Menu
  if (menuMode) {
    // Display menu options
    if (selectorMode == -1) {
      const char* menuStrings[] = {
          "MENU", 
          "Set alarm audio", 
          "Set alarm time", 
          "Set time", 
          "Set date"
      };

      UWORD yPos = 4;

      Paint_DrawString_EN(10, yPos, menuStrings[0], &Font12, BLACK, WHITE);

      Paint_DrawLine(8, yPos + 12, 120, yPos + 12, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
      
      for (int i = 1; i < 5; i++) {
          if (i == menuSelector) {
            Paint_DrawRectangle(8, yPos + 3 + (i * 11), 120, yPos + (i * 11) + 13, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
            Paint_DrawString_EN(10, yPos + 2 + (i * 11), menuStrings[i], &Font12, WHITE, BLACK);
          } else {
            Paint_DrawString_EN(10, yPos + 2 + (i * 11), menuStrings[i], &Font12, BLACK, WHITE);
          }
      } 
    } else if (selectorMode == 1) { // track select menu
      Paint_Clear(BLACK);

      const char* trackTitle = "TRACKS";
      const char* trackString = "Track ";

      UWORD yPos = 4;

      Paint_DrawString_EN(10, yPos, trackTitle, &Font12, BLACK, WHITE);

      Paint_DrawLine(8, yPos + 12, 120, yPos + 12, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
      
      for (int i = 1; i <= 4; i++) {
          if (i + windowStart - 1 == trackSelected) {
            Paint_DrawRectangle(8, yPos + 3 + (i * 11), 120, yPos + (i * 11) + 13, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
            
            if (allowBlink) {
              if (blinkState) {
                char displayString[25];
                snprintf(displayString, sizeof(displayString), "%s%d", trackString, i + windowStart - 1);
                Paint_DrawString_EN(10, yPos + 2 + (i * 11), displayString, &Font12, WHITE, BLACK);
              }
            } else {
              char displayString[25];
              snprintf(displayString, sizeof(displayString), "%s%d", trackString, i + windowStart - 1);
              Paint_DrawString_EN(10, yPos + 2 + (i * 11), displayString, &Font12, WHITE, BLACK);
            }
          } else {
            char displayString[25];
            snprintf(displayString, sizeof(displayString), "%s%d", trackString, i + windowStart - 1);
            Paint_DrawString_EN(10, yPos + 2 + (i * 11), displayString, &Font12, BLACK, WHITE);
          }
      } 
    } else if (selectorMode == 2) {
      Paint_Clear(BLACK);
      const char* on = "On";
      const char* off = "Off";
      char buf[9];
      int timerX = 6; 
      int timerY = 26;
      int switchX = 41;
      int switchY = 10;

      if (tempAlarmOn) {
        Paint_DrawRectangle(switchX, switchY + 1, switchX + 21, switchY + 11, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
        if (allowBlink && alarmSelectorMode == 1) {
          if (blinkState) {
            Paint_DrawString_EN(switchX + 3, switchY, on, &Font12, WHITE, BLACK);
          }
        } else {
          Paint_DrawString_EN(switchX + 3, switchY, on, &Font12, WHITE, BLACK);
        }
        Paint_DrawString_EN(switchX + 22, switchY, off, &Font12, BLACK, WHITE);
      } else {
        Paint_DrawRectangle(switchX + 23, switchY + 1, switchX + 44, switchY + 11, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
        Paint_DrawString_EN(switchX + 3, switchY, on, &Font12, WHITE, WHITE);
        if (allowBlink && alarmSelectorMode == 1) {
          if (blinkState) {
            Paint_DrawString_EN(switchX + 22, switchY, off, &Font12, WHITE, BLACK);
          }
        } else {
          Paint_DrawString_EN(switchX + 22, switchY, off, &Font12, WHITE, BLACK);
        }
      }
      Paint_DrawRectangle(switchX, switchY, switchX + 45, switchY + 11, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      Paint_DrawLine(switchX + 22, switchY + 1, switchX + 22, switchY + 11, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

      snprintf(buf, sizeof(buf), "%02d:%02d:%02d", tempAlarmHour, tempAlarmMinute, alarmSecond);
      for (int i = 0; i < 8; i++) {
          if (i < 2) {
            if (allowBlink && alarmSelectorMode == 2) {
              if (blinkState) {
                Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
              }
            } else {
              Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
            }
          } else if (i > 2 && i < 5) {
            if (allowBlink && alarmSelectorMode == 3) {
              if (blinkState) {
                Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
              }
            } else {
              Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
            }
          } else {
            Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
          }
          timerX += 14;
      }
    } else if (selectorMode == 3) {
      char buf[9];
      int timerX = 6; 
      int timerY = 23;

      snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hour, minutes, seconds);
      for (int i = 0; i < 8; i++) {
          if (i < 2) {
            if (allowBlink && timeSelectorMode == 1) {
              if (blinkState) {
                Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
              }
            } else {
              Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
            }
          } else if (i > 2 && i < 5) {
            if (allowBlink && timeSelectorMode == 2) {
              if (blinkState) {
                Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
              }
            } else {
              Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
            }
          } else if (i > 5 && i < 8) {
            if (allowBlink && timeSelectorMode == 3) {
              if (blinkState) {
                Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
              }
            } else {
              Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
            }
          } else {
            Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
          }
          timerX += 14;
      }
    } else if (selectorMode == 4) {
      int dateX = 6;
      int dateY = 23;
      char date[9];
      snprintf(date, sizeof(date), "%02d/%02d/%02d", tempDay, tempMonth, tempYear - 2000);
      for (int i = 0; i < 8; i++) {
        if (i < 2) {
          if (allowBlink && dateSelectorMode == 1) {
            if (blinkState) {
              Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
            }
          } else {
            Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
          }
        } else if (i > 2 && i < 5) {
          if (allowBlink && dateSelectorMode == 2) {
            if (blinkState) {
              Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
            }
          } else {
            Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
          }
        } else if (i > 5 && i < 8) {
          if (allowBlink && dateSelectorMode == 3) {
            if (blinkState) {
              Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
            }
          } else {
            Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
          }
        } else {
            Paint_DrawChar(dateX, dateY, (int)date[i], &Font24, WHITE, BLACK);
        }
        dateX += 14;
      }
    }
  } else {
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
    for (int i = 0; i < 8; i++) {
        Paint_DrawChar(timerX, timerY, (int)buf[i], &Font24, WHITE, BLACK);
        timerX += 14;
    }
  }

  // Update the OLED screen with the new image (text)
  OLED_2IN42_Display(BlackImage);
}

void initializePins() {
  pinMode(selectPin, INPUT_PULLUP);
  pinMode(plusPin, INPUT_PULLUP);
  pinMode(minusPin, INPUT_PULLUP);
  pinMode(menuPin, INPUT_PULLUP);
}

void initializeSerialCommunication() {
  // Initialize DFPlayer and Arduino Serial communication
  Serial1.begin(9600);
  delay(1000);  // Give time for the module to initialize
  Serial.begin(115200);
  delay(1000);
}

void initializeDFPlayer() {
  Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Initializing DFPlayer module ... Wait!"));
  delay(1000);

  if (!myDFPlayer.begin(Serial1, true, false)) {
    Serial.println(F("Not initialized:"));
    Serial.println(F("1. Check the DFPlayer Mini connections"));
    Serial.println(F("2. Insert an SD card"));
    while (true);  // Stop execution if initialization fails
  }

  Serial.println(F("DFPlayer Mini module initialized!"));

  // Set initial DFPlayer settings
  numberOfTracks = myDFPlayer.readFileCounts();
  myDFPlayer.setTimeOut(500);   // Set serial timeout to 500ms
  myDFPlayer.volume(volume);    // Set initial volume
  myDFPlayer.EQ(0);             // Set equalizer to normal
}

void initializeDisplay() {
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

void allocateBlackImageMemory() {
  Imagesize = ((OLED_2IN42_WIDTH % 8 == 0) ? (OLED_2IN42_WIDTH / 8) : (OLED_2IN42_WIDTH / 8 + 1)) * OLED_2IN42_HEIGHT;
  BlackImage = (UBYTE *)malloc(Imagesize);

  if (BlackImage == NULL) {
    Serial.print("Failed to apply for black memory...\r\n");
  }
  Serial.print("Paint_NewImage\r\n");
  Paint_NewImage(BlackImage, OLED_2IN42_WIDTH, OLED_2IN42_HEIGHT, 270, BLACK);
}

void initializeRTC() {
  // Initialize RTC and set the time
  RTC.begin();
  RTCTime startTime(6, Month::MARCH, 2025, 9, 0, 0, DayOfWeek::THURSDAY, SaveLight::SAVING_TIME_ACTIVE);
  RTC.setTime(startTime);

  Serial.println("Initializing RTC...");
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1); // Halt the program if RTC initialization fails
  } else {
    Serial.println("RTC initialized successfully");
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set RTC to compile time if lost power
  } else {
    Serial.println("RTC has power, no need to set time");
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

String dayToString(int day) {
  switch (day) {
    case 0: return "SUN";
    case 1: return "MON";
    case 2: return "TUE";
    case 3: return "WED";
    case 4: return "THU";
    case 5: return "FRI";
    case 6: return "SAT";
    default: return "???";
  }
}