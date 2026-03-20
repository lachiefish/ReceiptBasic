#include "ftp_server.h"
#include "config.h"
#include <WiFi.h>

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
