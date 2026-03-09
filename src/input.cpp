#include "input.h"
#include "config.h"
#include <Arduino.h>

void Input::begin()
{
  encoder.attachFullQuad(ENCODER_CLK_PIN, ENCODER_DT_PIN);
  encoder.clearCount();
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
  Serial.println(F("[INPUT] Encoder initialised"));
}

int Input::getCurrentCMC()
{
  int count = encoder.getCount() / 4;
  if (count != last_count)
  {
    if (count > last_count)
    {
      cmc++;
    }
    if (count < last_count)
    {
      cmc--;
    }
    last_count = count;
    Serial.print(F("[INPUT] Count: "));
    Serial.println(count);
  }
  cmc = constrainCMC(cmc);
  return cmc;
}

bool Input::isButtonPressed()
{
  bool current_button_state = digitalRead(ENCODER_SW_PIN);

  if (last_button_state == HIGH && current_button_state == LOW)
  {
    delay(50); // Debounce
    if (digitalRead(ENCODER_SW_PIN) == LOW)
    {
      Serial.println(F("[INPUT] Button pressed"));
      while (digitalRead(ENCODER_SW_PIN) == LOW)
        ; // Wait for release
      Serial.println(F("[INPUT] Button released"));
      last_button_state = HIGH;
      return true;
    }
  }
  last_button_state = current_button_state;
  return false;
}

int Input::constrainCMC(int cmc)
{
  if (cmc < 0)
    return 0;
  if (cmc > MAX_CMC)
    return MAX_CMC;
  return cmc;
}