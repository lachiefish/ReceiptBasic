#include "printer.h"
#include "config.h"
#include <Arduino.h>
#include <SD_MMC.h>

Printer::Printer() {}

void Printer::begin()
{
  Serial1.begin(PRINTER_BAUD, SERIAL_8N1, PRINTER_RX_PIN, PRINTER_TX_PIN);
}

void Printer::reset()
{
  Serial1.write(0x1b);
  Serial1.write(0x40); // Reset
}

void Printer::lineFeed(int num_lines)
{
  reset();
  for (int i = num_lines; i > 0; i--)
  {
    Serial1.write(0x0A); // Line feed
  }
}

void Printer::print(const String &text)
{
  reset();
  Serial1.print(text);
}

void Printer::printLine(const String &text)
{
  reset();
  print(text);
  lineFeed();
}

void Printer::printBitmapRaw(const String &image_path)
{
  File file = SD_MMC.open(image_path);
  if (!file)
  {
    Serial.print(F("[PRINTER] Failed to open image at: "));
    Serial.println(image_path);
    return;
  }

  bmpMode();

  for (int i = 0; i < 25680; i++) // 25680 = 48 * 535 (48 * 8px width, 535 height)
  {
    Serial1.write(~file.read());
  }

  file.close();

  lineFeed(3);
}

void Printer::bmpMode()
{
  reset();
  Serial1.write(0x1d); // GS -|
  Serial1.write(0x76); // v   |- Bitmap horizontal modulus data print
  Serial1.write(0x30); // 0  -|
  Serial1.write(0);    // Mode (0 = normal)
  Serial1.write(48);   // xL (width low byte) (48 bytes wide = 48*8 pixels = 384)
  Serial1.write(0);    // xH (width high byte)
  Serial1.write(23);   // yL (height low byte)
  Serial1.write(2);    // yH (height high byte) (e.g. 2*256 + 23 = height)
}

Printer printer;