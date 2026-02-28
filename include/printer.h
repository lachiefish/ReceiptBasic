#pragma once
#include <Arduino.h>

class Printer
{
public:
  Printer();
  void begin();
  void printBitmapRaw(const String &image_path);

private:
};

extern Printer printer;