#include <Arduino.h>
#include "display.h"
#include "input.h"
#include "storage.h"
#include "printer.h"
#include "web_server.h"

void setup()
{
  Serial.begin(115200);
  storage.begin();
  display.begin();
  display.clear();
  input.begin();
  printer.begin();
  web_server.begin();
}

// Util
String cardPathToCardName(const String &card_path)
{
  int start = card_path.lastIndexOf('/') + 1;
  int end = card_path.lastIndexOf('.');
  return card_path.substring(start, end);
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
  if (input.isButtonPressed() && !printer.busy())
  {
    display.invertDisplay(true);
    String card_path = storage.getRandomCardPath(String(current_cmc));
    display.invertDisplay(false);
    display.showTimedMessage(cardPathToCardName(card_path), 2000, 2);
    Serial.print("[MAIN] Printing file: ");
    Serial.println(card_path);
    printer.printBitmapRaw(card_path);
  }
}