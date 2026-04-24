
#include "web_server.h"
#include "printer.h"
#include "storage.h"
#include "config.h"
#include <freertos/task.h>
#include <ElegantOTA.h>
// #include <WebSerial.h>

#include "pages/index.h"
#include "pages/status.h"

PrinterWebServer::PrinterWebServer(Storage &storage, Printer &printer)
    : server(80), _storage(storage), _printer(printer)
{
}

void PrinterWebServer::begin()
{
  WiFi.softAP("ReceiptBasic", WEB_SERVER_PASSWORD);
  Serial.println(F("[WEB SERVER] WiFi started"));
  ElegantOTA.begin(&server);
  // WebSerial.begin(&server);
  server.begin();
  setupRoutes();
  Serial.println(F("[WEB SERVER] Web Server started"));
}

void PrinterWebServer::update() {
  // WebSerial.loop();
}

void PrinterWebServer::setupRoutes()
{
  server.on("/print", HTTP_POST, [this](AsyncWebServerRequest *request)
            { handlePrint(request); });

  server.on("/api/cards", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleGetCards(request); });

  server.on("/api/preview", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handlePreview(request); });

  server.on("/api/status", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleStatus(request); });

  server.on("/status", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleStatusPage(request); });

  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
            { handleIndex(request); });

  Serial.println(F("[WEB SERVER] Routes registered"));
}

// POST /print — prints the bitmap at the given SD path
void PrinterWebServer::handlePrint(AsyncWebServerRequest *request)
{
  if (!request->hasParam("path", true))
  {
    Serial.println(F("[WEB SERVER] POST /print — missing 'path' param"));
    request->send(400, "application/json", "{\"error\":\"Missing path\"}");
    return;
  }

  if (_printer.busy())
  {
    Serial.println(F("[WEB SERVER] POST /print — busy, rejecting"));
    request->send(409, "application/json", "{\"error\":\"Printer is busy\"}");
    return;
  }

  String path = request->getParam("path", true)->value();

  if (!_storage.fileExists(path))
  {
    Serial.println(F("[WEB SERVER] POST /print — path does not exist"));
    request->send(400, "application/json", "{\"error\":\"Path does not exist\"}");
    return;
  }

  Serial.print(F("[WEB SERVER] POST /print — "));
  Serial.println(path);
  request->send(200, "application/json", "{\"status\":\"printed\"}");

  struct PrintTaskParams
  {
    String path;
    Storage &storage;
    Printer &printer;
  };

  PrintTaskParams *params = new PrintTaskParams{path, _storage, _printer};
  xTaskCreate([](void *param)
              {
    PrintTaskParams *p = static_cast<PrintTaskParams *>(param);
    Serial.print(F("[WEB SERVER] Print task started for: "));
    Serial.println(p->path);

    uint8_t *buffer = nullptr;
    size_t len = p->storage.readFileBytes(p->path, buffer);
    if (len > 0 && buffer)
    {
      p->printer.printBitmapRaw(buffer, len);
      free(buffer);
    }
    else
    {
      Serial.println(F("[WEB SERVER] Failed to read image file"));
    }

    Serial.println(F("[WEB SERVER] Print task finished"));
    delete p;
    vTaskDelete(NULL); }, "print_task", 8192, params, 1, NULL);
}

// GET /api/cards — serves index.txt; optional ?cmc=N for creature folders
void PrinterWebServer::handleGetCards(AsyncWebServerRequest *request)
{
  String path;
  if (request->hasParam("cmc"))
  {
    String cmc = request->getParam("cmc")->value();
    path = String(CARDS_PATH) + "/" + cmc + "/index.txt";
    Serial.print(F("[WEB SERVER] GET /api/cards?cmc="));
    Serial.println(cmc);
  }
  else
  {
    path = String(TOKENS_PATH) + "/index.txt";
    Serial.println(F("[WEB SERVER] GET /api/cards (tokens)"));
  }

  if (!_storage.fileExists(path))
  {
    request->send(404, "text/plain", "File not found");
    return;
  }
  request->send(_storage.getFS(), path, "text/plain");
}

// GET /api/preview?path=/tokens/Foo.bin — serves the raw .bin file from SD
void PrinterWebServer::handlePreview(AsyncWebServerRequest *request)
{
  if (!request->hasParam("path"))
  {
    request->send(400, "text/plain", "Missing path");
    return;
  }
  String path = request->getParam("path")->value();
  Serial.print(F("[WEB SERVER] GET /api/preview?path="));
  Serial.println(path);

  if (!_storage.fileExists(path))
  {
    request->send(404, "text/plain", "File not found");
    return;
  }
  request->send(_storage.getFS(), path, "application/octet-stream");
}

// GET /api/status — returns system info as JSON
void PrinterWebServer::handleStatus(AsyncWebServerRequest *request)
{
  Serial.println(F("[WEB SERVER] GET /api/status"));

  // Uptime
  unsigned long ms = millis();
  unsigned long secs = ms / 1000;
  unsigned long mins = secs / 60;
  unsigned long hrs = mins / 60;
  unsigned long days = hrs / 24;

  // Memory
  uint32_t freeHeap = ESP.getFreeHeap();
  uint32_t totalHeap = ESP.getHeapSize();
  uint32_t minFreeHeap = ESP.getMinFreeHeap();
  uint32_t freePsram = ESP.getFreePsram();
  uint32_t totalPsram = ESP.getPsramSize();

  // SD card
  uint64_t sdTotal = _storage.totalBytes();
  uint64_t sdUsed = _storage.usedBytes();

  // WiFi
  int clients = WiFi.softAPgetStationNum();

  // Chip
  uint32_t flashSize = ESP.getFlashChipSize();
  uint32_t sketchSize = ESP.getSketchSize();
  uint32_t sketchFree = ESP.getFreeSketchSpace();
  uint8_t cpuFreq = ESP.getCpuFreqMHz();

  // Build JSON
  String json = "{";
  json += "\"uptime_ms\":" + String(ms);
  json += ",\"uptime_str\":\"" + String(days) + "d " + String(hrs % 24) + "h " + String(mins % 60) + "m " + String(secs % 60) + "s\"";
  json += ",\"heap_free\":" + String(freeHeap);
  json += ",\"heap_total\":" + String(totalHeap);
  json += ",\"heap_min_free\":" + String(minFreeHeap);
  json += ",\"psram_free\":" + String(freePsram);
  json += ",\"psram_total\":" + String(totalPsram);
  json += ",\"sd_total\":" + String((uint32_t)(sdTotal / 1048576));
  json += ",\"sd_used\":" + String((uint32_t)(sdUsed / 1048576));
  json += ",\"wifi_clients\":" + String(clients);
  json += ",\"flash_size\":" + String(flashSize / 1024);
  json += ",\"sketch_size\":" + String(sketchSize / 1024);
  json += ",\"sketch_free\":" + String(sketchFree / 1024);
  json += ",\"cpu_freq\":" + String(cpuFreq);
  json += ",\"chip_model\":\"" + String(ESP.getChipModel()) + "\"";
  json += ",\"chip_cores\":" + String(ESP.getChipCores());
  json += ",\"chip_revision\":" + String(ESP.getChipRevision());
  json += ",\"sdk_version\":\"" + String(ESP.getSdkVersion()) + "\"";
  json += "}";

  request->send(200, "application/json", json);
}

// GET /status — serves the status monitoring page
void PrinterWebServer::handleStatusPage(AsyncWebServerRequest *request)
{
  Serial.println(F("[WEB SERVER] GET /status"));
  request->send(200, "text/html", STATUS_HTML);
}

// GET / — serves the search UI
void PrinterWebServer::handleIndex(AsyncWebServerRequest *request)
{
  Serial.println(F("[WEB SERVER] GET /"));
  request->send(200, "text/html", INDEX_HTML);
}
