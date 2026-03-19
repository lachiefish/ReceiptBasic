#include "power.h"
#include "config.h"

void Power::begin()
{
  pinMode(BATTERY_SENSE_PIN, INPUT);
  analogReadResolution(12);
  analogSetPinAttenuation(BATTERY_SENSE_PIN, ADC_11db);

  Serial.print(F("[POWER] Battery sensing initialised on GPIO "));
  Serial.println(BATTERY_SENSE_PIN);
}

void Power::update()
{
  const unsigned long now = millis();
  if (valid_reading && (now - last_sample_time_ms) < POWER_SAMPLE_INTERVAL_MS)
  {
    return;
  }

  float measured_voltage = readRawBatteryVoltage();
  if (measured_voltage <= 0.0f)
  {
    return;
  }

  if (!valid_reading)
  {
    battery_voltage = measured_voltage;
  }
  else
  {
    battery_voltage += (measured_voltage - battery_voltage) * POWER_FILTER_ALPHA;
  }

  valid_reading = true;
  last_sample_time_ms = now;
}

float Power::getBatteryVoltage()
{
  return battery_voltage;
}

bool Power::hasValidReading()
{
  return valid_reading;
}

float Power::readRawBatteryVoltage()
{
  uint32_t total_millivolts = 0;
  int valid_samples = 0;

  for (int i = 0; i < POWER_SAMPLE_COUNT; i++)
  {
    int millivolts = analogReadMilliVolts(BATTERY_SENSE_PIN);
    if (millivolts > 0)
    {
      total_millivolts += static_cast<uint32_t>(millivolts);
      valid_samples++;
    }
    delay(2);
  }

  if (valid_samples == 0)
  {
    return -1.0f;
  }

  float adc_voltage = (total_millivolts / static_cast<float>(valid_samples)) / 1000.0f;
  float pack_voltage = adc_voltage * getDividerRatio() * BATTERY_CALIBRATION_FACTOR;

  if (pack_voltage < 0.0f)
  {
    return 0.0f;
  }
  return pack_voltage;
}

float Power::getDividerRatio()
{
  return (BATTERY_DIVIDER_R_TOP_OHMS + BATTERY_DIVIDER_R_BOTTOM_OHMS) / BATTERY_DIVIDER_R_BOTTOM_OHMS;
}