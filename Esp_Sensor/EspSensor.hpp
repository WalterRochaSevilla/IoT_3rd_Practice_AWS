#pragma once
#include "Mqtt.hpp"
#include "MagneticSensor.hpp"

class EspSensor {
    private:
        MagneticSensor* doorSensor;
        MQTTClient* mqtt;
        NetworkConfig* networkConfig;
        NetworkController* net;
        MQTTConfig* mqttConfig;
        static EspSensor* instance;
        void handleMessage(char* topic, byte* payload, unsigned int length) {
            Serial.print("Mensaje recibido [");
            Serial.print(topic);
            Serial.print("]: ");
            Serial.println((char)payload[0]);
        }
    public:
        EspSensor(int sensorPin, const char* ssid, const char* password, const char* server, int port,
                    const char* publishTopic, const char* subscribeTopic, const char* clientId) {
            instance = this;
            networkConfig = new NetworkConfig(ssid, password);
            net = new NetworkController(*networkConfig);
            doorSensor = new MagneticSensor(sensorPin);
            mqttConfig = new MQTTConfig(server, port, publishTopic, subscribeTopic, clientId,
                                        [](char* topic, byte* payload, unsigned int length) {
                                            if (instance) instance->handleMessage(topic, payload, length);
                                        });
            mqtt = new MQTTClient(*net, *mqttConfig);
        }
        void setup() {
            Serial.begin(115200);
            doorSensor->begin();
            mqtt->initialize();
        }
        void loop() {
            mqtt->loop();
            if (doorSensor->hasStateChanged()) {
                if (doorSensor->getLastState()) {
                    mqtt->publish("OPEN");
                } else {
                    mqtt->publish("CLOSED");
                }
            }
        }
};
EspSensor* EspSensor::instance = nullptr;

