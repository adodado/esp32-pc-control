#include <WiFi.h>
#include <PubSubClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Update these with values suitable network.
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";
const char* mqtt_server = "192.168.1.100";

WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer server(80);

int relayPin = 14;
String switch1;
String strTopic;
String strPayload;

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

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  strTopic = String((char*)topic);
  if(strTopic == "workstation/cmnd/POWER")
    {
    switch1 = String((char*)payload);
    if(switch1 == "ON")
      {
        Serial.println("POWER");
        digitalWrite(relayPin, HIGH);
        delay(200);
        digitalWrite(relayPin, LOW);
      }
    else if(switch1 == "OFF")
      {
        Serial.println("POWER");
        digitalWrite(relayPin, HIGH);
        delay(200);
        digitalWrite(relayPin, LOW);
      }
    else
      {
        Serial.println("BAD MESSAGE");
      }
    }
    else if(strTopic == "workstation/cmnd/RESET"){
    switch1 = String((char*)payload);
    if(switch1 == "HARD_SHUTDOWN")
      {
        Serial.println("HARD_SHUTDOWN");
        digitalWrite(relayPin, HIGH);
        delay(7000);
        digitalWrite(relayPin, LOW);
      }
    else
      {
        Serial.println("BAD MESSAGE");
      }
    }
    else
      {
        Serial.println("BAD MESSAGE");
      }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, subscribe to everything under the workstation topic.
      client.subscribe("workstation/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}
 
void setup()
{
  Serial.begin(115200);
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(relayPin, OUTPUT);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/on", HTTP_GET, [] (AsyncWebServerRequest *request) {
    digitalWrite(relayPin, HIGH);
    delay(200);
    request->send(200, "text/plain", "ok");
  });

  server.on("/off", HTTP_GET, [] (AsyncWebServerRequest *request) {
    digitalWrite(relayPin, LOW);
    request->send(200, "text/plain", "ok");
  });
    server.on("/hard_shutdown_on", HTTP_GET, [] (AsyncWebServerRequest *request) {
    digitalWrite(relayPin, HIGH);
    delay(7000);
    request->send(200, "text/plain", "ok");
  });
    server.on("/hard_shutdown_off", HTTP_GET, [] (AsyncWebServerRequest *request) {
    digitalWrite(relayPin, LOW);
    request->send(200, "text/plain", "ok");
  });
  server.onNotFound(notFound);
  server.begin();
}
 
void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
