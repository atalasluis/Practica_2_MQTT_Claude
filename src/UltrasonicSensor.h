#pragma once
#include <Arduino.h>

class UltrasonicSensor {
private:
    int trigPin;
    int echoPin;

    float medirSimple();
    void ordenar(float arr[], int n);

public:
    UltrasonicSensor(int trig, int echo);
    void init();
    long medir(); // ahora usa mediana
};