#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "globals.h"
#include "DFPlayerManager.h"
#include "DisplayManager.h"
#include "ButtonManager.h"
#include "RTCManager.h"

void setup() 
{
  initializePins();
  initializeSerialCommunication();
  initializeDFPlayer();
  initializeDisplay();
  initializeRTC();
}

void loop() 
{
  setRTC();
  handleAlarms();
  handleButtons();
  updateUI();
}

void initializePins() {
  pinMode(selectPin, INPUT_PULLUP);
  pinMode(plusPin, INPUT_PULLUP);
  pinMode(minusPin, INPUT_PULLUP);
  pinMode(menuPin, INPUT_PULLUP);
}

void initializeSerialCommunication() 
{
  Serial1.begin(9600);
  delay(1000);  
  Serial.begin(115200);
  delay(1000);
}

void handleAlarms()
{
  if (alarmOn && !menuMode && alarmHour == hour && alarmMinute == minutes && alarmSecond == seconds) doAlarm();
}

void doAlarm()
{
  myDFPlayer.play(alarmID);
  alarmPlaying = true;
  if (!screenOn) 
  {
    screenOn = true;
    turnOnScreen();
  }
}