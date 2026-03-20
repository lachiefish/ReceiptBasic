#include "display.h"
#include "config.h"
#include <Wire.h>

Display::Display() : oled(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, OLED_RESET)
{
}

void Display::begin()
{
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN); // Initialise I2C communication
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS))
  {
    Serial.println(F("[DISPLAY] SSD1306 allocation failed"));
    return;
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
  int cmc_size = 3;
  int title_size = 1;
  int battery_size = 1;
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);

  oled.setTextSize(title_size);
  String title = "MOMIR BASIC";
  oled.setCursor(getCenteredX(title, title_size), 0);
  oled.print(title);

  oled.setTextSize(cmc_size);
  String cmc_text = "CMC: " + cmc;
  oled.setCursor(getCenteredX(cmc_text, cmc_size), 18);
  oled.print(cmc_text);

  oled.setTextSize(battery_size);
  String battery_text = battery_voltage >= 0.0f
                            ? "BAT: " + String(battery_voltage, 2) + "V / " + String(battery_voltage / BATTERY_CELL_COUNT, 2) + "V"
                            : "BAT: --.-V";
  oled.setCursor(getCenteredX(battery_text, battery_size), OLED_SCREEN_HEIGHT - (OLED_CHAR_HEIGHT * battery_size));
  oled.print(battery_text); // #TODO refactor battery voltage display

  oled.display();
}

void Display::showCMC(int cmc)
{
  showCMC(String(cmc));
}

void Display::setBatteryVoltage(float voltage)
{
  battery_voltage = voltage;
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
  oled.println(wordWrap(text, size));
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
  String wrapped = text;
  unsigned int text_length = text.length();
  int line_char_length = OLED_SCREEN_WIDTH / (OLED_CHAR_WIDTH * size);

  int last_space_index = -1;
  int current_line_length = 0;
  for (unsigned int i = 0; i < text_length; i++)
  {
    char c = wrapped.charAt(i);

    if (c == '\n')
    {
      current_line_length = 0;
      last_space_index = -1;
      continue;
    }

    current_line_length++;

    if (c == ' ')
    {
      last_space_index = i;
    }

    if (current_line_length > line_char_length && last_space_index != -1)
    {
      wrapped.setCharAt(last_space_index, '\n');
      last_space_index = -1;
      current_line_length = 0;
      i = last_space_index;
    }
  }

  return wrapped;
}

int Display::getCenteredX(const String &text, int size)
{
  int char_width = OLED_CHAR_WIDTH * size; // 6 pixels per char at size 1
  int text_width = text.length() * char_width;
  int x = (OLED_SCREEN_WIDTH - text_width) / 2;
  return x < 0 ? 0 : x;
}