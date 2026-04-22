#include "MQTTManager.h"

MQTTManager::MQTTManager(const char* server, int port)
    : server(server), port(port), client(espClient) {}

void MQTTManager::init() {
    client.setServer(server, port);
}

void MQTTManager::connect() {

    if (client.connected()) return;

    Serial.print("Conectando a MQTT...");

    String clientId = "ESP32_LUIS_";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
        Serial.println("conectado");

        client.subscribe("casa/led/control");

    } else {
        Serial.print("Error: ");
        Serial.println(client.state());
    }
}

void MQTTManager::loop() {

    if (!client.connected()) {
        connect();
    }

    client.loop();
}

void MQTTManager::publish(const char* topic, const char* message) {
    client.publish(topic, message);
}

void MQTTManager::setCallback(void (*callback)(char*, byte*, unsigned int)) {
    client.setCallback(callback);
}

void MQTTManager::subscribe(const char* topic) {
    client.subscribe(topic);
}