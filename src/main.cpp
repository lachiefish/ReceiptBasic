#include <Arduino.h>
#include "display.h"
#include "input.h"
#include "storage.h"

void setup()
{
  Serial.begin(115200);
  storage.begin();
  display.begin();
  display.clear();
  input.begin();
}

void loop()
{
  int current_cmc = input.getCurrentCMC();
  display.showCMC(String(current_cmc));
  if (input.isButtonPressed())
  {
    display.invertDisplay(true);
    delay(300);
    display.invertDisplay(false);
    String file = storage.getRandomCard(String(current_cmc));
    Serial.print("Printing file: ");
    Serial.println(file);
  }
}