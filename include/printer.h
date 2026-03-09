#pragma once
#include <Arduino.h>
#include <atomic>

class Printer
{
public:
  Printer();
  void begin();
  void reset();
  void lineFeed(int num_lines = 1);
  void print(const String &text);
  void printLine(const String &text);
  void printBitmapRaw(const uint8_t *data, size_t length);
  bool busy() const;

private:
  std::atomic<bool> _busy{false};

  void bmpMode();
};