#pragma once

class FTPServer
{
public:
  void begin();
  void update();

private:
  bool started = false;
};
