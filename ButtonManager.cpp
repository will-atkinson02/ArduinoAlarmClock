#include "ButtonManager.h"
#include "Globals.h"
#include <Arduino.h>

void handleButtons() {
    if (isButtonPressed(selectPin, selectStatePrevious)) handleSelectButton();
    if (isButtonPressed(plusPin, plusStatePrevious)) handlePlusButton();
    if (isButtonPressed(minusPin, minusStatePrevious)) handleMinusButton();
    if (isButtonPressed(menuPin, menuStatePrevious)) handleMenuButton();
}

bool isButtonPressed(int pin, bool &previousState) {
    bool current = digitalRead(pin);
    bool pressed = (current == LOW && previousState == HIGH);
    previousState = current;
    return pressed;
}

void handleSelectButton()
{
  if (!screenOn) 
  {
    screenOn = true;
    turnOnScreen();
    return;
  }
  else if (!alarmPlaying && !menuMode) 
  {
    screenOn = false;
    turnOffScreen();
    return;
  } 
  
  if (alarmPlaying) 
  {
    alarmPlaying = false;
    myDFPlayer.stop();
    return;
  }

  if (!menuMode) return;
  
  if (selectorMode == SELECTOR_NONE) 
  { 
    selectorMode = static_cast<SelectorMode>(menuSelector);

    switch (selectorMode) 
    {
      case SELECTOR_TRACK:
        trackSelected = 1;
        windowStart = 1;
        windowEnd = 4;
        break;
      case SELECTOR_ALARM:
        alarmSelectorMode = 1;
        allowBlink = true;
        tempAlarmOn = alarmOn;
        tempAlarmHour = alarmHour;
        tempAlarmMinute = alarmMinute;
        break;
      case SELECTOR_TIME:
        timeSelectorMode = 1;
        allowBlink = true;
        tempHour = hour;
        tempMinutes = minutes;
        tempSeconds = seconds;
        break;
      case SELECTOR_DATE:
        dateSelectorMode = 1;
        allowBlink = true;
        tempDay = dayInt;
        tempMonth = month;
        tempYear = year;
        break;
      default:
        break;
    }
    return;
  }
  
  switch (selectorMode)
  {
    case SELECTOR_TRACK:
      if (testTrackPlaying) 
      {
        // confirm changes and return to home screen
        alarmID = trackSelected;
        myDFPlayer.stop();
        menuMode = false;
        selectorMode = SELECTOR_NONE;
        trackSelected = -1;
        testTrackPlaying = false;
        allowBlink = false;
      } 
      else 
      {
        testTrackPlaying = true;
        allowBlink = true;
        myDFPlayer.play(trackSelected);
      }
      break;
    case SELECTOR_ALARM:
      if (alarmSelectorMode < 3) 
      {
        alarmSelectorMode++;
      } 
      else 
      {
        alarmOn = tempAlarmOn;
        alarmHour = tempAlarmHour;
        alarmMinute = tempAlarmMinute;
        selectorMode = SELECTOR_NONE;
        allowBlink = false;
      }
      break;
    case SELECTOR_TIME:
      if (timeSelectorMode < 3) 
      {
        timeSelectorMode++;
      } 
      else 
      {
        selectorMode = SELECTOR_NONE;
        allowBlink = false;
        rtc.adjust(DateTime(year, month, dayInt, tempHour, tempMinutes, tempSeconds));
      }
      break;
    case SELECTOR_DATE:
      if (dateSelectorMode < 3) 
      {
        dateSelectorMode++;
      } 
      else 
      {
        selectorMode = SELECTOR_NONE;
        allowBlink = false;
        rtc.adjust(DateTime(tempYear, tempMonth, tempDay, hour, minutes, seconds));
      }
  }
}

void handlePlusButton()
{
  if (!screenOn)
  {
    screenOn = true;
    turnOnScreen();
    return;
  }
  
  if (!menuMode && volume < MAX_VOLUME)
  {
    myDFPlayer.volumeUp();
    volume++;
    return;
  }
  switch (selectorMode)
  {
    case SELECTOR_NONE:
      menuSelector = (menuSelector % 4) + 1;
      break;
    case SELECTOR_TRACK:
      if (testTrackPlaying && volume < MAX_VOLUME) 
      {
        myDFPlayer.volumeUp();
        break;
      } 

      if (numberOfTracks <= 4) 
      {
        trackSelected++;
        if (trackSelected > numberOfTracks) trackSelected = 1;
      } 
      else 
      {
        if (trackSelected == numberOfTracks) 
        {
          windowStart = 1;
          windowEnd = 4;
          trackSelected = 1;
        } 
        else 
        {
          if (trackSelected == windowEnd && trackSelected < numberOfTracks) 
          {
              windowStart++;
              windowEnd++;
          }
          trackSelected++;
        }
      }
      break;
    case SELECTOR_ALARM:
      switch (alarmSelectorMode)
      {
        case 1:
          tempAlarmOn = !tempAlarmOn;
          break;
        case 2:
          tempAlarmHour = (tempAlarmHour + 1) % 24;
          break;
        case 3:
          tempAlarmMinute = (tempAlarmMinute + 1) % 60;
      }
      break;
    case SELECTOR_TIME:
      switch (timeSelectorMode)
      {
        case 1:
          tempHour = (tempHour + 1) % 24;
          break;
        case 2:
          tempMinutes = (tempMinutes + 1) % 60;
          break; 
        case 3:
          tempSeconds = (tempSeconds + 1) % 60;
      }
      break;
    case SELECTOR_DATE:
      switch (dateSelectorMode)
      {
        case 1: 
          tempDay = (tempDay % 30) + 1;
          break;
        case 2:
          tempMonth = (tempMonth % 12) + 1;
          break;
        case 3: 
          tempYear++;
          if (tempYear > 2099) tempYear = 2000;
      }
      break;
  }
}

void handleMinusButton()
{
  if (!screenOn)
  {
    screenOn = true;
    turnOnScreen();
    return;
  }

  if (!menuMode && volume > MIN_VOLUME) 
  {
      myDFPlayer.volumeDown();
      volume--;
      return;
  }
  switch (selectorMode)
  {
    case SELECTOR_NONE:
      menuSelector = (menuSelector + 2) % 4 + 1;
      break;
    case SELECTOR_TRACK:
      if (testTrackPlaying && volume > MIN_VOLUME) 
      {
        myDFPlayer.volumeDown();
        break;
      } 

      if (numberOfTracks <= 4) 
      {
        trackSelected--;
        if (trackSelected < 1) trackSelected = numberOfTracks;
      } 
      else 
      {
        if (trackSelected == 1) 
        {
          windowStart = numberOfTracks - 3;
          windowEnd = numberOfTracks;
          trackSelected = numberOfTracks;
        } 
        else 
        {
          if (trackSelected == windowStart && windowStart > 1) 
          {
              windowStart--;
              windowEnd--;
          }
          trackSelected--;
        }
      }
      break;
    case SELECTOR_ALARM:
      switch (alarmSelectorMode)
      {
        case 1:
          tempAlarmOn = !tempAlarmOn;
          break;
        case 2:
          tempAlarmHour = tempAlarmHour == 0 ? 23 : tempAlarmHour - 1;
          break;
        case 3:
          tempAlarmMinute = tempAlarmMinute == 0 ? 59 : tempAlarmMinute - 1;
      }
      break;
    case SELECTOR_TIME:
      switch (timeSelectorMode)
      {
        case 1:
          tempHour = tempHour == 0 ? 23 : tempHour - 1;
          break;
        case 2:
          tempMinutes = tempMinutes == 0 ? 59 : tempMinutes - 1;
          break;
        case 3:
          tempSeconds = tempSeconds == 0 ? 59 : tempSeconds - 1;
      }
      break;
    case SELECTOR_DATE:
      switch (dateSelectorMode)
      {
        case 1:
          tempDay = tempDay == 0 ? 30 : tempDay - 1;
          break;
        case 2: 
          tempMonth = tempMonth == 0 ? 12 : tempMonth - 1;
          break;
        case 3:
          tempYear = tempYear == 2000 ? 2099 : tempYear - 1;
      }
      break;
  }
}

void handleMenuButton()
{
  if (!screenOn)
  {
    screenOn = true;
    turnOnScreen();
    return;
  }

  if (selectorMode == SELECTOR_NONE) 
  {
    menuMode = !menuMode;
    menuSelector = 1; 
  } 
  else if (selectorMode == SELECTOR_TRACK) 
  {
    if (testTrackPlaying) 
    {
      myDFPlayer.stop();
      allowBlink = false;
      testTrackPlaying = false;
    } 
    else 
    {
      selectorMode = SELECTOR_NONE;
    }
  } 
  else if (selectorMode == SELECTOR_ALARM) 
  {
    if (alarmSelectorMode > 1) 
    {
      alarmSelectorMode--;
      
    } 
    
    else 
    
    {
      selectorMode = SELECTOR_NONE;
      allowBlink = false;
    }
  } else if (selectorMode == SELECTOR_TIME) 
  {
    selectorMode = SELECTOR_NONE;
    allowBlink = false;
  } 
  else if (selectorMode == SELECTOR_DATE) 
  {
    if (dateSelectorMode > 1) 
    {
      dateSelectorMode--;
    } 
    else 
    {
      selectorMode = SELECTOR_NONE;
      allowBlink = false;
    }
  }
}