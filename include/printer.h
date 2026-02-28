#pragma once
#include <Arduino.h>

class Printer
{
public:
  Printer();
  void begin();
  void reset();
  void lineFeed(int num_lines = 1);
  void print(const String &text);
  void printLine(const String &text);
  void printBitmapRaw(const String &image_path);

private:
  void bmpMode();
};

extern Printer printer;