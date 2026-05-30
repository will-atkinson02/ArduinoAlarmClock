#include "globals.h"

// Display
bool screenOn = true;
UBYTE *BlackImage;
UWORD Imagesize;

// DFPlayer
DFRobotDFPlayerMini myDFPlayer;
int volume = 10;
int pause = 0;
int alarmID = 1;
int numberOfTracks = 0;

// track selector
int trackSelected = -1;
bool testTrackPlaying = false;
int windowStart = 1;
int windowEnd = 4;

//buttons
const int plusPin = 2;
const int minusPin = 3;
const int selectPin = 4;
const int menuPin = 5;
bool selectStatePrevious = HIGH;
bool plusStatePrevious = HIGH;
bool minusStatePrevious = HIGH;
bool menuStatePrevious = HIGH;

//RTC/Date
RTC_DS3231 rtc;
int year = 2025;
int month = 1;
int dayInt = 1;
int hour = 0;
int minutes = 0;
int seconds = 0;
String dayStr = "Wed";
bool rtcOn = false;

// menu
bool menuMode = false;
int menuSelector;

// alarm settings
bool alarmPlaying = false;
bool alarmOn = false;
int alarmHour = 9;
int alarmMinute = 1;
int alarmSecond = 0;

// alarm selector
int alarmSelectorMode, tempAlarmHour, tempAlarmMinute;
bool tempAlarmOn;

// time selector
int timeSelectorMode, tempHour, tempMinutes, tempSeconds;

// date selector
int dateSelectorMode, tempDay, tempMonth, tempYear;

// blink animation
bool allowBlink = false;
unsigned long previousMillis = 0;
const long onDuration = 600;
const long offDuration = 500;
bool blinkState = true;

SelectorMode selectorMode = SELECTOR_NONE;