#include "display.h"
#include "config.h"
#include <Wire.h>

Display display;

Display::Display() : oled(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, OLED_RESET)
{
}

void Display::begin()
{
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN); // Initialise I2C communication
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS))
  {
    Serial.println(F("[DISPLAY] SSD1306 allocation failed"));
    for (;;)
      ;
  }
  Serial.println(F("[DISPLAY] SSD1306 initialised"));
}

void Display::update()
{
  if (showing_timed_message && millis() >= message_end_time)
  {
    showing_timed_message = false;
    oled.clearDisplay();
  }
}

void Display::clear()
{
  oled.clearDisplay();
}

void Display::invertDisplay(bool inverted)
{
  oled.invertDisplay(inverted);
}

void Display::showCMC(const String &cmc)
{
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  String title = "MOMIR BASIC";
  oled.setCursor(getCenteredX(title, 1), 0);
  oled.print(title);
  oled.setTextSize(3);
  String cmc_text = "CMC: " + cmc;
  oled.setCursor(getCenteredX(cmc_text, 3), 20);
  oled.print(cmc_text);
  oled.display();
}

void Display::showCMC(int cmc)
{
  showCMC(String(cmc));
}

void Display::showTimedMessage(const String &text, unsigned long duration_ms, int size, bool centered)
{
  oled.clearDisplay();
  oled.setTextSize(size);
  if (centered)
  {
    oled.setCursor(getCenteredX(text, size), 16);
  }
  else
  {
    oled.setCursor(0, 16);
  }
  oled.println(text);
  oled.display();

  showing_timed_message = true;
  message_end_time = millis() + duration_ms;
}

bool Display::isShowingTimedMessage()
{
  return showing_timed_message;
}

void Display::dimDisplay(bool dim)
{
  oled.dim(dim);
}

String Display::wordWrap(const String &text, int size)
{
  return text;
}

int Display::getCenteredX(const String &text, int size)
{
  int char_width = OLED_CHAR_WIDTH * size; // 6 pixels per char at size 1
  int text_width = text.length() * char_width;
  int x = (OLED_SCREEN_WIDTH - text_width) / 2;
  return x < 0 ? 0 : x;
}