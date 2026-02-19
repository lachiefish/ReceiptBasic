#include <Arduino.h>
#include "display.h"
#include "input.h"

void setup()
{
  Serial.begin(115200);
  display.begin();
  display.showCmc("test");
  input.begin();
}

void loop()
{
  display.showCmc(String(input.getCurrentCMC()));
  if (input.isButtonPressed()) {
    display.invertDisplay(true);
    delay(300);
    display.invertDisplay(false);
  }
}