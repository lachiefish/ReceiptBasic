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
  void showTimedMessage(const String &message, unsigned long duration_ms);

  bool isShowingTimedMessage();

private:
  Adafruit_SSD1306 oled;
  bool showing_timed_message = false;
  unsigned long message_end_time = 0;

  int getCenteredX(const String &text, int textSize);
};

extern Display display;