#include "EspSensor.hpp"

EspSensor* espSensor = new EspSensor(4, "Matheo", "cg4hts55fh4fyr7", "a2rmxu7hc5rdsd-ats.iot.us-east-2.amazonaws.com", 8883, "ESP_CLIENT_SENSOR", "$aws/things/IoT_thing/shadow/update", "$aws/things/IoT_thing/shadow/update/delta");

void setup() {
    espSensor->setup();
}
void loop() {
    espSensor->loop();
}