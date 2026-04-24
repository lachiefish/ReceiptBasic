#include "printer.h"
#include "config.h"
#include <Arduino.h>

Printer::Printer() {}

void Printer::begin()
{
  Serial1.begin(PRINTER_BAUD, SERIAL_8N1, PRINTER_RX_PIN, PRINTER_TX_PIN);
  reset();
  Serial.println(F("[PRINTER] Printer initialised"));
}

bool Printer::busy() const { return _busy.load(); }

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

void Printer::printBitmapRaw(const uint8_t *data, size_t length)
{
  _busy.store(true);

  constexpr size_t kPrinterRowBytes = 48; // 384px wide printer / 8 pixels per byte
  auto reverseBits = [](uint8_t b) -> uint8_t
  {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
  };

  if (!data || length == 0)
  {
    Serial.println(F("[PRINTER] No image data provided"));
    _busy.store(false);
    return;
  }

  bmpMode();

  Serial.print(F("[PRINTER] Writing "));
  Serial.print(length);
  Serial.println(F(" bytes to serial"));

  if (length % kPrinterRowBytes != 0)
  {
    // Fallback to linear write if the payload is not an exact whole number of rows.
    for (size_t i = 0; i < length; i++)
    {
      Serial1.write(~data[i]);
    }
  }
  else
  {
    const size_t rowCount = length / kPrinterRowBytes;

    // Send rows bottom-to-top and mirror each row so output reads left-to-right.
    for (size_t row = rowCount; row > 0; --row)
    {
      const size_t rowOffset = (row - 1) * kPrinterRowBytes;
      for (size_t colByte = kPrinterRowBytes; colByte > 0; --colByte)
      {
        const uint8_t src = data[rowOffset + (colByte - 1)];
        Serial1.write(~reverseBits(src));
      }
    }
  }

  lineFeed(3);

  _busy.store(false);
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
  Serial.println(F("[PRINTER] Printer set to BMP mode"));
}