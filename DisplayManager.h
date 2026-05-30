#pragma once

void initializeDisplay();
void allocateBlackImageMemory();
void updateUI();
void updateDisplay();
void drawDefaultUI();
void drawMenuOptions();
void drawTrackSelector();
void drawAlarmSelector();
void drawTimeSelector();
void drawDateSelector();
void handleTextBlink();
void drawBell(int bellX, int bellY);
void drawDayAndNightIcons(int sunX, int sunY, int hour);