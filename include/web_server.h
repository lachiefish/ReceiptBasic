#pragma once
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class Storage;
class Printer;

class WebServer
{
public:
  WebServer(Storage &storage, Printer &printer);
  void begin();

private:
  AsyncWebServer server;
  Storage &_storage;
  Printer &_printer;

  void setupRoutes();
  void handlePrint(AsyncWebServerRequest *request);
  void handleGetCards(AsyncWebServerRequest *request);
  void handlePreview(AsyncWebServerRequest *request);
  void handleStatus(AsyncWebServerRequest *request);
  void handleStatusPage(AsyncWebServerRequest *request);
  void handleIndex(AsyncWebServerRequest *request);
};