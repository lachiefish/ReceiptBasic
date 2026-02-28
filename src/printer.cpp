#include "printer.h"
#include "config.h"
#include <Arduino.h>
#include <SD_MMC.h>

Printer::Printer() {}

void Printer::begin()
{
  Serial1.begin(PRINTER_BAUD, SERIAL_8N1, PRINTER_RX_PIN, PRINTER_TX_PIN);
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

  Serial1.write(0x1b);
  Serial1.write(0x40); // Reset

  Serial1.write(0x1d); // GS -|
  Serial1.write(0x76); // v   |- Bitmap horizontal modulus data print
  Serial1.write(0x30); // 0  -|
  Serial1.write(0);    // Mode (0 = normal)
  Serial1.write(48);   // xL (width low byte) (48 bytes wide = 48*8 pixels = 384)
  Serial1.write(0);    // xH (width high byte)
  Serial1.write(23);   // yL (height low byte)
  Serial1.write(2);    // yH (height high byte) (e.g. 2*256 + 23 = height)

  for (int i = 0; i < 25680; i++) // 25680 = 48 * 535
  {
    Serial1.write(~file.read());
  }

  file.close();

  Serial1.write(0x0A); // Line feed
  Serial1.write(0x0A);
  Serial1.write(0x0A);
  Serial1.write(0x0A);
}

Printer printer;