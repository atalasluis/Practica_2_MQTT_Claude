#pragma once
#include <PubSubClient.h>
#include <WiFi.h>

class MQTTManager {
private:
    const char* server;
    int port;

    WiFiClient espClient;
    PubSubClient client;

public:
    MQTTManager(const char* server, int port);

    void init();
    void connect();
    void loop();

    void publish(const char* topic, const char* message);

    void setCallback(void (*callback)(char*, byte*, unsigned int));
    void subscribe(const char* topic);
};