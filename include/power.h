#pragma once
#include <Arduino.h>

class Power
{
public:
  void begin();
  void update();
  float getBatteryVoltage();
  bool hasValidReading();

private:
  float battery_voltage = 0.0f;
  bool valid_reading = false;
  unsigned long last_sample_time_ms = 0;

  float getDividerRatio();
  float readRawBatteryVoltage();
};