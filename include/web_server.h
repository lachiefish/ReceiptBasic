#pragma once
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class WebServer
{
public:
  WebServer();
  void begin();

private:
  AsyncWebServer server;

  void setupRoutes();
};

extern WebServer web_server;