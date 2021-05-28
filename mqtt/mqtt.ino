#include <WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable network.
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";
const char* mqtt_server = "192.168.1.100";
// const char* state_topic = "workstation/tele/STATE";
// const char* availability_topic = "workstation/tele/LWT";
// const char* power_switch_config_topic = "homeassistant/switch/workstation-power/config";
// const char* hard_shutdown_config_topic = "homeassistant/switch/workstation-power/config";
// const String hass_config = "{\"name\":\"Workstation-Power\",\"command_topic\":\"workstation/cmnd/POWER\",\"state_topic\":\"workstation/tele/STATE\",\"value_template\":\"{{value_json.POWER}}\",\"payload_off\":\"OFF\",\"payload_on\":\"ON\",\"availability_topic\":\"workstation/tele/LWT\",\"payload_available\":\"Online\",\"payload_not_available\":\"Offline\",\"unique_id\":\"2EB32AF_RL_1\",\"device\":{\"identifiers\":[\"2EB32AF\"]},\"platform\":\"mqtt\"}";

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
}
 
void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
