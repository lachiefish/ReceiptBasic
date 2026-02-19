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
      ; // Don't proceed, loop forever
  }
  Serial.println(F("[DISPLAY] SSD1306 initialised"));
}

void Display::update()
{
}

void Display::clear()
{
  oled.clearDisplay();
}

void Display::invertDisplay(bool inverted) {
  oled.invertDisplay(inverted);
}

void Display::showCmc(String cmc)
{
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.println("CMC: " + cmc);
  oled.display();
}