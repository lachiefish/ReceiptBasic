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
  void showCMC(int cmc);
  void showTimedMessage(const String &text, unsigned long duration_ms, int size = 1, bool centered = true);
  void dimDisplay(bool dim);

  bool isShowingTimedMessage();

private:
  Adafruit_SSD1306 oled;
  bool showing_timed_message = false;
  unsigned long message_end_time = 0;

  String wordWrap(const String& text, int size);

  int getCenteredX(const String &text, int size);
};