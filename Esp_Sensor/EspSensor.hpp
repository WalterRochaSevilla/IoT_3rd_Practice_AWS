#pragma once
#include "Mqtt.hpp"
#include "MagneticSensor.hpp"
#include "ArduinoJson.h"

class EspSensor {
    private:
        MagneticSensor* doorSensor;
        MqttClient* mqtt;
        NetworkConfig* networkConfig;
        NetworkHandler* net;
        MqttConfig* mqttConfig;
        static EspSensor* instance;
        const char* publishTopic;
        const char* subscribeTopic;
    public:
        EspSensor(int sensorPin, const char* ssid, const char* password, const char* server, int port, const char* clientId, const char* publishTopic, const char* subscribeTopic) {
            instance = this;
            this->publishTopic = publishTopic;
            this->subscribeTopic = subscribeTopic;
            networkConfig = new NetworkConfig(ssid, password);
            net = new NetworkHandler(networkConfig);
            doorSensor = new MagneticSensor(sensorPin);
            mqttConfig = new MqttConfig(server, clientId, 
                [](char* topic, uint8_t* payload, unsigned int length) {
                    Serial.print("Mensaje recibido [");
                    Serial.print(topic);
                    Serial.print("]: ");
                    Serial.println((char)payload[0]);
                }
                , port);
            mqtt = new MqttClient( mqttConfig,net);
        }
        void setup() {
            Serial.begin(115200);
            doorSensor->begin();
            mqtt->initialize();
        }
        void loop() {
            mqtt->loop();
            if (doorSensor->hasStateChanged()) {
                JsonDocument doc;
                doc["state"]["desired"]["exteriorDoor"] = doorSensor->getLastState() ? "OPEN" : "CLOSE";
                mqtt->publish(publishTopic, doc.as<String>().c_str());
            }
        }
};
EspSensor* EspSensor::instance = nullptr;

