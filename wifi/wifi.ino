#include <WiFi.h>
#include <AsyncTCP.h>

#include <ESPAsyncWebServer.h>

const char* ssid = "WIFI SSID";
const char* password = "WIFI PASSWORD";

const int output = 14;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <title>ESP32 PC Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 30px;}
      .button {
        padding: 10px 20px;
        font-size: 24px;
        text-align: center;
      min-width: 250px;
        outline: none;
        color: #fff;
        background-color: #2f4468;
        border: none;
        border-radius: 5px;
        box-shadow: 0 6px #999;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }  
      .button:hover {background-color: #1f2e45}
      .button:active {
        background-color: #1f2e45;
        box-shadow: 0 4px #666;
        transform: translateY(2px);
      }
    </style>
  </head>
  <body>
    <h1>ESP32 PC Control</h1>
    <button class="button" onmousedown="toggleCheckbox('on');" ontouchstart="toggleCheckbox('on');" onmouseup="toggleCheckbox('off');" ontouchend="toggleCheckbox('off');">Power Button</button>
        <br><br>
        <button class="button" onmousedown="toggleCheckbox('hard_shutdown_on');" ontouchstart="toggleCheckbox('hard_shutdown_on');" onmouseup="toggleCheckbox('hard_shutdown_off');" ontouchend="toggleCheckbox('hard_shutdown_off');">Hard Shutdown</button>
    <br><br>
        <button class="button" onmousedown="toggleCheckbox('sleep_on');" ontouchstart="toggleCheckbox('sleep_on');" onmouseup="toggleCheckbox('sleep_off');" ontouchend="toggleCheckbox('sleep_off');">Sleep Mode</button>
   <script>
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
   }
  </script>
  </body>
</html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("ESP IP Address: http://");
  Serial.println(WiFi.localIP());
  
  pinMode(output, OUTPUT);
  digitalWrite(output, LOW);
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/on", HTTP_GET, [] (AsyncWebServerRequest *request) {
    digitalWrite(output, HIGH);
    delay(500);
    request->send(200, "text/plain", "ok");
  });

  server.on("/off", HTTP_GET, [] (AsyncWebServerRequest *request) {
    digitalWrite(output, LOW);
    request->send(200, "text/plain", "ok");
  });
      server.on("/sleep_on", HTTP_GET, [] (AsyncWebServerRequest *request) {
    digitalWrite(output, HIGH);
    delay(1000);
    request->send(200, "text/plain", "ok");
  });
    server.on("/sleep_off", HTTP_GET, [] (AsyncWebServerRequest *request) {
    digitalWrite(output, LOW);
    request->send(200, "text/plain", "ok");
  });
    server.on("/hard_shutdown_on", HTTP_GET, [] (AsyncWebServerRequest *request) {
    digitalWrite(output, HIGH);
    delay(8000);
    request->send(200, "text/plain", "ok");
  });
    server.on("/hard_shutdown_off", HTTP_GET, [] (AsyncWebServerRequest *request) {
    digitalWrite(output, LOW);
    request->send(200, "text/plain", "ok");
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
 
}
