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
  void handlePrint(AsyncWebServerRequest *request);
  void handleGetCards(AsyncWebServerRequest *request);
  void handlePreview(AsyncWebServerRequest *request);
  void handleStatus(AsyncWebServerRequest *request);
  void handleStatusPage(AsyncWebServerRequest *request);
  void handleIndex(AsyncWebServerRequest *request);
};

extern WebServer web_server;