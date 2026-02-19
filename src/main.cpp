#include <Arduino.h>
#include "display.h"
#include "input.h"

void setup()
{
  Serial.begin(115200);
  display.begin();
  display.showCMC("test");
  input.begin();
}

void loop()
{
  display.showCMC(String(input.getCurrentCMC()));
  if (input.isButtonPressed()) {
    display.invertDisplay(true);
    delay(300);
    display.invertDisplay(false);
  }
}