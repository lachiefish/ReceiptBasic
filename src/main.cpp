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
  display.update(); // Update display state?

  if (display.isShowingTimedMessage())
  {
    return; // No input but loop still runs
  }

  int current_cmc = input.getCurrentCMC();
  display.showCMC(String(current_cmc));
  if (input.isButtonPressed())
  {
    display.invertDisplay(true);
    String file = storage.getRandomCardPath(String(current_cmc));
    display.invertDisplay(false);
    display.showTimedMessage(file.substring(7), 2000);
    Serial.print("[MAIN] Printing file: ");
    Serial.println(file);
  }
}