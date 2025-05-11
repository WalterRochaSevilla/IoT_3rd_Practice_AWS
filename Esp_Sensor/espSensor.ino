#include <Arduino.h>
#include <ArduinoJson.h>
#include "Mqtt.hpp"
#define LED_BUILT_IN 2

NetworkConfig* networkConfig = new NetworkConfig("Matheo", "cg4hts55fh4fyr7");
NetworkHandler* networkHandler = new NetworkHandler(networkConfig);
JsonDocument doc;
MqttClient* mqttClient = new MqttClient();
MqttConfig* mqttConfig = new MqttConfig("a2rmxu7hc5rdsd-ats.iot.us-east-2.amazonaws.com", "ESP_CLIENT_SENSOR",
  [](char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]: ");
    
    // Crear un buffer para el mensaje
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    
    // Crear un documento JSON para parsear el mensaje
    JsonDocument jsonDoc;  // Reemplaza StaticJsonDocument<200>
    DeserializationError error = deserializeJson(jsonDoc, message);
    if (error) {
        Serial.print("Error parseando JSON: ");
        Serial.println(error.c_str());
        return;
    }    
    // Mostrar el JSON formateadoú
    Serial.println("JSON:");
    serializeJsonPretty(jsonDoc, Serial);
    int ledDesiredState = jsonDoc["state"]["led_builtin"];
    jsonDoc["state"]["reported"]["led_builtin"] = ledDesiredState;
    Serial.println(ledDesiredState);
    if(ledDesiredState == 0){
      digitalWrite(LED_BUILT_IN, LOW);
    }else{
      digitalWrite(LED_BUILT_IN, HIGH);
    }
    JsonDocument updateDoc;
    updateDoc["state"]["reported"]["led_builtin"] = ledDesiredState;
    mqttClient->publish("$aws/things/IoT_thing/shadow/update", updateDoc.as<String>().c_str());
    Serial.println();
  },8883);

void setup() {
  Serial.begin(115200);
  mqttClient->setAll(mqttConfig, networkHandler);
  networkHandler->inicialize();
  networkHandler->connect();
  mqttClient->reconnect();
  mqttClient->subscribe("$aws/things/IoT_thing/shadow/update/delta");
  pinMode(LED_BUILT_IN, OUTPUT);
  doc["state"]["reported"]["led_builtin"] = 0;
  mqttClient->publish("$aws/things/IoT_thing/shadow/update", doc.as<String>().c_str());
}


void loop() {
  mqttClient->loop();
  delay(1000);
}