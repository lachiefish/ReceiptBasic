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
  void showCmc(String cmc);

private:
  Adafruit_SSD1306 oled;
};

extern Display display;