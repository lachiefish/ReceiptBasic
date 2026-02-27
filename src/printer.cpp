#include "printer.h"
#include "config.h"
#include <Arduino.h>

Printer::Printer() : thermal(&Serial1) {}

void Printer::begin()
{
  Serial1.begin(PRINTER_BAUD, SERIAL_8N1, PRINTER_RX_PIN, PRINTER_TX_PIN);
  thermal.begin();

  thermal.println("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

  thermal.inverseOn();
  thermal.println(F("Inverse ON"));
  thermal.inverseOff();

  thermal.println();
  thermal.println();
}

void Printer::printCardBitmap(int width, int height, byte bmp[])
{
  thermal.printBitmap(width, height, bmp);
}

Printer printer;