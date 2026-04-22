#include "UltrasonicSensor.h"

UltrasonicSensor::UltrasonicSensor(int trig, int echo)
    : trigPin(trig), echoPin(echo) {}

void UltrasonicSensor::init() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

// Medición simple (básica)
float UltrasonicSensor::medirSimple() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(3);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duracion = pulseIn(echoPin, HIGH, 30000);

    if (duracion == 0) return -1;

    float distancia = (duracion * 0.0343) / 2;
    return distancia;
}

// Ordenamiento (bubble sort)
void UltrasonicSensor::ordenar(float arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                float temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// Medición robusta (MEJORADA)
long UltrasonicSensor::medir() {

    const int muestras = 7;
    float valores[muestras];
    int validas = 0;

    for (int i = 0; i < muestras; i++) {

        float d = medirSimple();

        if (d > 0 && d < 400) {
            valores[validas++] = d;
        }

        delay(15);
    }

    if (validas == 0) return -1;

    ordenar(valores, validas);

    float mediana = valores[validas / 2];

    return (long)mediana;
}

