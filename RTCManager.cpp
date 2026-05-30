#include "RTCManager.h"
#include "Globals.h"
#include <RTClib.h>
#include <Wire.h>

void initializeRTC() 
{
  rtcOn = false;
  Serial.println("Initializing RTC...");

  Wire.begin();
  delay(100);

  if (!rtc.begin()) 
  {
    Serial.println("Couldn't find RTC");
    return;
  } 

  Serial.println("RTC initialized successfully");
  rtcOn = true;

  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, setting time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set RTC to compile time if lost power
  } 
  else 
  {
    Serial.println("RTC has power, no need to set time");
  }
}

void setRTC()
{
  if (selectorMode == SELECTOR_TIME) 
  {
    hour = tempHour;
    minutes = tempMinutes;
    seconds = tempSeconds;
  } 
  else if (rtcOn)
  {
    DateTime now = rtc.now();
    hour = now.hour();
    minutes = now.minute();
    seconds = now.second();
    year = now.year();

    month = now.month();
    dayInt = now.day();
    dayStr = dayToString(now.dayOfTheWeek());
  }
  else
  {
    hour = 0;
    minutes = 0;
    seconds = 0;
    year = 2000;
    month = 1;
    dayInt = 1;
    dayStr = "MON";
  }
}

const char* dayToString(int day) {
  switch(day) {
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