#pragma once
#include "DEV_Config.h"
#include "OLED_Driver.h"
#include "GUI_paint.h"
#include "DFRobotDFPlayerMini.h"
#include <RTClib.h>

// Display
extern bool screenOn;
extern UBYTE *BlackImage;
extern UWORD Imagesize;

// DFPlayer
extern DFRobotDFPlayerMini myDFPlayer;
extern int volume;
extern int numberOfTracks;
extern int alarmID;

// Track navigation
extern bool testTrackPlaying;
extern int trackSelected;
extern int windowStart, windowEnd;

//buttons
extern const int plusPin;
extern const int minusPin;
extern const int selectPin;
extern const int menuPin;
extern bool selectStatePrevious;
extern bool plusStatePrevious;
extern bool minusStatePrevious;
extern bool menuStatePrevious;

// menu
extern bool menuMode;
extern int menuSelector;

// alarm settings
extern bool alarmPlaying;
extern bool alarmOn;
extern int alarmHour;
extern int alarmMinute;
extern int alarmSecond;

// Alarm
extern bool tempAlarmOn;
extern int tempAlarmHour, tempAlarmMinute;

//RTC/Date
extern RTC_DS3231 rtc;
extern int year, month, dayInt, hour, minutes, seconds;
extern String dayStr;
extern bool rtcOn;

// Time/Date
extern int tempHour, tempMinutes, tempSeconds;
extern int tempDay, tempMonth, tempYear;

// Menu selector submodes
extern int timeSelectorMode;
extern int dateSelectorMode;
extern int alarmSelectorMode;

// blink animation
extern bool allowBlink;
extern unsigned long previousMillis;
extern const long onDuration;
extern const long offDuration;
extern bool blinkState;

enum SelectorMode 
{
  SELECTOR_NONE = -1,
  SELECTOR_TRACK = 1,
  SELECTOR_ALARM = 2,
  SELECTOR_TIME = 3,
  SELECTOR_DATE = 4
};

extern SelectorMode selectorMode;

#define MAX_VOLUME 20
#define MIN_VOLUME 0