#pragma once

#define FTP_SERVER_NETWORK_TYPE DEFAULT_FTP_SERVER_NETWORK_TYPE_ESP32
#define STORAGE_TYPE STORAGE_SD_MMC
#include <SimpleFTPServer.h>

class FTPServer
{
public:
  void begin();
  void update();

private:
  FtpServer ftp_server_instance;
  bool started = false;
};
