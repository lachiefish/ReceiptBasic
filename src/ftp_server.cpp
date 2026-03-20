#include "ftp_server.h"

#include <Arduino.h>
#include <WiFi.h>

#include "config.h"

#define FTP_SERVER_NETWORK_TYPE DEFAULT_FTP_SERVER_NETWORK_TYPE_ESP32
#define STORAGE_TYPE STORAGE_SD_MMC
#include <SimpleFTPServer.h>

namespace
{
  FtpServer ftp_server_instance;
}

void FTPServer::begin()
{
  ftp_server_instance.begin(FTP_USERNAME, FTP_PASSWORD);
  started = true;

  Serial.println(F("[FTP] FTP server started"));
  Serial.print(F("[FTP] Username: "));
  Serial.println(FTP_USERNAME);
}

void FTPServer::update()
{
  if (!started)
  {
    return;
  }

  if (WiFi.softAPgetStationNum() <= 0)
  {
    return;
  }

  ftp_server_instance.handleFTP();
}
