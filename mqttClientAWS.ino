#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* WIFI_SSID = "CATOLAB";
const char* WIFI_PASS = "sistemas2024";

// Replace with your MQTT broker details
const char* MQTT_BROKER = "a2x84s7vsr52x5-ats.iot.us-east-2.amazonaws.com"; // changeMe
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "ESP-32";

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----
)KEY";

// Replace with your think shadow details
const char* UPDATE_TOPIC = "$aws/things/iot_object/shadow/update";              // publish changeMe
const char* UPDATE_DELTA_TOPIC = "$aws/things/iot_object/shadow/update/delta";  // subscribe changeMe

WiFiClientSecure wiFiClient;
PubSubClient client(wiFiClient);

StaticJsonDocument<JSON_OBJECT_SIZE(64)> inputDoc;
StaticJsonDocument<JSON_OBJECT_SIZE(4)> outputDoc;
char outputBuffer[128];

byte ledBuiltIn = 0;

void reportLedBuiltIn() {
  outputDoc["state"]["reported"]["led_builtin"] = ledBuiltIn;
  serializeJson(outputDoc, outputBuffer);
  client.publish(UPDATE_TOPIC, outputBuffer);
}

void setLedBuiltIn() {
  if (ledBuiltIn) digitalWrite(LED_BUILTIN, HIGH);
  else digitalWrite(LED_BUILTIN, LOW);
  reportLedBuiltIn();
}

// Callback function to handle messages received from the subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) message += String((char) payload[i]);
  Serial.println("Message from topic " + String(topic) + ":" + message);
  DeserializationError err = deserializeJson(inputDoc, payload);
  if (!err) {
    if (String(topic) == UPDATE_DELTA_TOPIC) {
      ledBuiltIn = inputDoc["state"]["led_builtin"].as<int8_t>();
      setLedBuiltIn();
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  setupWiFi();

  wiFiClient.setCACert(AMAZON_ROOT_CA1);
  wiFiClient.setCertificate(CERTIFICATE);
  wiFiClient.setPrivateKey(PRIVATE_KEY);

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
}

void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CLIENT_ID)) {
      Serial.println("connected");
      // Subscribe to a topic
      client.subscribe(UPDATE_DELTA_TOPIC);
      Serial.println("Subscribed to " + String(UPDATE_DELTA_TOPIC));
      delay(100);
      reportLedBuiltIn();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
