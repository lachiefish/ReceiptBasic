#pragma once
#include <Adafruit_SSD1306.h>

class Display
{
public:
  Display();
  void begin();
  void update();
  void clear();
  void invertDisplay(bool inverted);
  void showCMC(const String &cmc);

private:
  Adafruit_SSD1306 oled;

  int getCenteredX(const String &text, int textSize);
};

extern Display display;