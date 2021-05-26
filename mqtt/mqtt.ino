#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable network.
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);
int relayPin = 14;
String switch1;
String strTopic;
String strPayload;

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
  if(strTopic == "esp32/switch1")
    {
    switch1 = String((char*)payload);
    if(switch1 == "POWER")
      {
        Serial.println("POWER");
        digitalWrite(relayPin, HIGH);
        delay(500);
        digitalWrite(relayPin, LOW);
      }
    else if(switch1 == "SLEEP")
      {
        Serial.println("SLEEP");
        digitalWrite(relayPin, HIGH);
        delay(2000);
        digitalWrite(relayPin, LOW);
      }
    else if(switch1 == "HARD_SHUTDOWN")
      {
        Serial.println("HARD_SHUTDOWN");
        digitalWrite(relayPin, HIGH);
        delay(8000);
        digitalWrite(relayPin, LOW);
      }
    else
      {
        Serial.println("BAD MESSAGE");
      }
    }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("esp32/#");
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
}
 
void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}