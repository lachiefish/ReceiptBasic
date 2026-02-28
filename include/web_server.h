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
  void printToken(const String &token);
};

extern WebServer web_server;