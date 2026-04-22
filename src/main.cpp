#include <Arduino.h>
#include "LEDController.h"
#include "UltrasonicSensor.h"
#include "WiFiManager.h"
#include "MQTTManager.h"

// Pines (ajusta si necesitas)
#define TRIG 14
#define ECHO 12

#define LED_R 19
#define LED_Y 18
#define LED_G 5

WiFiManager wifi("TP-Link_22C2", "69562495");
MQTTManager mqtt("broker.hivemq.com", 1883);
/// ver informacion del sensor topic: casa/sensor/distancia
/// control de actuadores topic: casa/led/control


LEDController led(LED_R, LED_Y, LED_G);
UltrasonicSensor sensor(TRIG, ECHO);

unsigned long lastMsg = 0;

void callback(char* topic, byte* payload, unsigned int length) {

    Serial.println("📩 CALLBACK ACTIVADO");
    
    String message = "";

    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.print("Mensaje recibido: ");
    Serial.println(message);

    // 🔥 CONTROL DE LEDs
    if (message == "GREEN") {
        led.setMode(GREEN);
    }
    else if (message == "YELLOW") {
        led.setMode(YELLOW);
    }
    else if (message == "RED") {
        led.setMode(RED);
    }
    else if (message == "DANGER") {
        led.setMode(DANGER);
    }
}

void setup() {
    Serial.begin(115200);

    led.init();
    sensor.init();

    wifi.connect();
    mqtt.init();
    mqtt.connect(); // 🔥 importante

    mqtt.setCallback(callback);
    mqtt.subscribe("casa/led/control");
}

void loop() {

    wifi.reconnect();
    mqtt.loop();

    long distancia = sensor.medir();

    Serial.print("Distancia: ");
    Serial.println(distancia);

    // 📤 Publicar cada 1s
    if (distancia > 0 && millis() - lastMsg > 1000) {
        lastMsg = millis();

        String msg = String(distancia);
        mqtt.publish("casa/sensor/distancia", msg.c_str());
    }

    led.update();
}