#include "DFPlayerManager.h"
#include "Globals.h"
#include "OLED_Driver.h"
#include "GUI_paint.h"
#include "DEV_Config.h"
#include "Debug.h"
#include "ImageData.h"

void initializeDFPlayer() { 
Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Initializing DFPlayer module ... Wait!"));
  delay(1000);

  if (!myDFPlayer.begin(Serial1, true, false)) 
  {
    Serial.println(F("Not initialized:"));
    Serial.println(F("1. Check the DFPlayer Mini connections"));
    Serial.println(F("2. Insert an SD card"));
    while (true);  // Stop execution if initialization fails
  }

  Serial.println(F("DFPlayer Mini module initialized!"));

  // Set initial DFPlayer settings
  numberOfTracks = myDFPlayer.readFileCounts();

  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(volume);
}