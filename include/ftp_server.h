#pragma once

#include <SimpleFTPServer.h>

class FTPServer
{
public:
  void begin();
  void update();

private:
  FtpServer ftp_server_instance;
};
