#include <Arduino.h>
#include "display.h"
#include "input.h"
#include "storage.h"
#include "printer.h"
#include "power.h"
#include "web_server.h"
#include "ftp_server.h"

Storage storage;
Display display;
Input input;
Printer printer;
Power power;
PrinterWebServer web_server(storage, printer);
FTPServer ftp_server;

void setup()
{
  Serial.begin(115200);
  storage.begin();
  display.begin();
  display.clear();
  power.begin();
  input.begin();
  printer.begin();
  web_server.begin();
  ftp_server.begin();
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
  web_server.update();
  ftp_server.update();

  power.update();
  if (power.hasValidReading())
  {
    display.setBatteryVoltage(power.getBatteryVoltage());
  }

  display.update(); // Update display state?

  if (display.isShowingTimedMessage())
  {
    return; // No input but loop still runs
  }

  int current_cmc = input.getCurrentCMC();
  display.showCMC(current_cmc);
  if (input.isButtonPressed() && !printer.busy())
  {
    display.invertDisplay(true);
    String card_path = storage.getRandomCardPath(String(current_cmc));
    display.invertDisplay(false);
    display.showTimedMessage(cardPathToCardName(card_path), 2000, 2);
    Serial.print("[MAIN] Printing file: ");
    Serial.println(card_path);

    uint8_t *buffer = nullptr;
    size_t len = storage.readFileBytes(card_path, buffer);
    if (len > 0 && buffer)
    {
      printer.printBitmapRaw(buffer, len);
      free(buffer);
    }
    else
    {
      Serial.println(F("[MAIN] Failed to read image file"));
    }
  }
}