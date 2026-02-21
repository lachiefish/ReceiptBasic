#pragma once
#include <ESP32Encoder.h>

class Input
{
public:
  void begin();
  int getCurrentCMC();
  bool isButtonPressed();

private:
  ESP32Encoder encoder;
  int last_count = 0;
  int cmc = 0;
  bool last_button_state = true;

  int constrainCMC(int cmc);
};

extern Input input;