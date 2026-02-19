#include "input.h"
#include "config.h"
#include <Arduino.h>

Input input;

void Input::begin()
{
  encoder.attachFullQuad(ENCODER_CLK_PIN, ENCODER_DT_PIN);
  encoder.clearCount();
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
  Serial.println(F("[INPUT] Encoder initialised"));
}

int Input::getCurrentCMC() // #TODO dont track CMC with count, when constrained it doesn't go down until count goes below MAX_CMC
{
  static int last_count = 0;
  int count = encoder.getCount() / 4;
  if (count != last_count)
  {
    last_count = count;
    Serial.print(F("[INPUT] Count: "));
    Serial.println(count);
  }
  int cmc = constrainCMC(count);
  return constrainCMC(count);
}

bool Input::isButtonPressed()
{
  static bool lastState = HIGH;
  bool currentState = digitalRead(ENCODER_SW_PIN);

  if (lastState == HIGH && currentState == LOW)
  {
    delay(50); // Debounce
    if (digitalRead(ENCODER_SW_PIN) == LOW)
    {
      Serial.println(F("[INPUT] Button pressed"));
      while (digitalRead(ENCODER_SW_PIN) == LOW)
        ; // Wait for release
      Serial.println(F("[INPUT] Button released"));
      lastState = HIGH;
      return true;
    }
  }
  lastState = currentState;
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