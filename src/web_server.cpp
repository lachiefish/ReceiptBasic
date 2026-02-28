#include "web_server.h"
#include "printer.h" // #TODO: Need to fix this tight coupling

WebServer::WebServer() : server(80)
{
}

void WebServer::begin()
{
  WiFi.softAP("ReceiptBasic", "magicslotmachine");
  server.begin();
  setupRoutes();
}

void WebServer::setupRoutes()
{
  server.on("/print", HTTP_POST, [](AsyncWebServerRequest *request)
            {

  if (!request->hasParam("token", true)) {
    request->send(400, "application/json", "{\"error\":\"Missing token\"}");
    return;
  }

  String token = request->getParam("token", true)->value();

  printer.printLine(token);
  printer.lineFeed(2);

  request->send(200, "application/json", "{\"status\":\"printed\"}"); });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Token Printer</title>
  <style>
    body { font-family: Arial; text-align:center; margin-top:50px; }
    input, button { padding:10px; font-size:18px; }
    #status { margin-top:20px; font-weight:bold; }
  </style>
</head>
<body>
  <h1>Print Token</h1>
  <input id="token" placeholder="Enter Token">
  <button onclick="printToken()">Print</button>
  <div id="status"></div>

<script>
function printToken() {
  let token = document.getElementById("token").value;

  fetch("/print", {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: "token=" + encodeURIComponent(token)
  })
  .then(res => res.json())
  .then(data => {
    document.getElementById("status").innerText = "Printed!";
  })
  .catch(err => {
    document.getElementById("status").innerText = "Error!";
  });
}
</script>
</body>
</html>
)rawliteral"); });
}

WebServer web_server;