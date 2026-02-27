#pragma once
#include <Adafruit_Thermal.h>

class Printer
{
public:
  Printer();
  void begin();
  void printCardBitmap(int width, int height, byte bmp[]);

private:
  Adafruit_Thermal thermal;
};

extern Printer printer;