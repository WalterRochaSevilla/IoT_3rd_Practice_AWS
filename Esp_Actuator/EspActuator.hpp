#pragma once
#include "Mqtt.hpp"
#include "ServoController.hpp"

class EspActuator {
private:
    ServoController* servoController;
    MQTTClient* mqtt;
    NetworkConfig* networkConfig;
    NetworkController* net;
    MQTTConfig* mqttConfig;
    static EspActuator* instance;

    static void mqttCallback(char* topic, byte* payload, unsigned int length) {
        if (instance) instance->handleMessage(topic, payload, length);
    }

    void handleMessage(char* topic, byte* payload, unsigned int length) {
        Serial.print("Mensaje recibido [");
        Serial.print(topic);
        Serial.print("]: ");
        Serial.println((char)payload[0]);
        
        if (payload[0] == '0') {
            servoController->open();
            Serial.println("Turning ON the actuator");
        } else if (payload[0] == '1') {
            servoController->close();
            Serial.println("Turning OFF the actuator");
        } else {
            Serial.println("Invalid command received.");
        }
    }

public:
    EspActuator(byte actuatorPin, const char* ssid, const char* password, const char* server, int port,
                   const char* publishTopic, const char* subscribeTopic, const char* clientId) {
        instance = this;
        networkConfig = new NetworkConfig(ssid, password);
        net = new NetworkController(*networkConfig);
        servoController = new ServoController(actuatorPin);
        mqttConfig = new MQTTConfig(server, port, publishTopic, subscribeTopic, clientId, &mqttCallback);
        mqtt = new MQTTClient(*net, *mqttConfig);
    }

    void setup() {
        Serial.begin(115200);
        mqtt->initialize();
        servoController->begin();
        mqtt->subscribe();
    }

    void loop() {
        mqtt->loop();
    }
};

EspActuator* EspActuator::instance = nullptr;