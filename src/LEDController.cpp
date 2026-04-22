#include "LEDController.h"

LEDController::LEDController(int r, int y, int g)
    : redPin(r), yellowPin(y), greenPin(g),
      currentMode(OFF), lastBlink(0), blinkState(false) {}

void LEDController::init() {
    pinMode(redPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(greenPin, OUTPUT);

    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);
}

// Configura modo
void LEDController::setMode(Mode mode) {

    currentMode = mode;

    switch(mode) {

        case GREEN:
            digitalWrite(greenPin, HIGH);
            digitalWrite(yellowPin, LOW);
            digitalWrite(redPin, LOW);
            break;

        case YELLOW:
            digitalWrite(greenPin, HIGH);
            digitalWrite(yellowPin, HIGH);
            digitalWrite(redPin, LOW);
            break;

        case RED:
            digitalWrite(greenPin, HIGH);
            digitalWrite(yellowPin, HIGH);
            digitalWrite(redPin, HIGH);
            break;

        case DANGER:
            lastBlink = millis();
            blinkState = false;
            break;

        case OFF:
            digitalWrite(greenPin, LOW);
            digitalWrite(yellowPin, LOW);
            digitalWrite(redPin, LOW);
            break;
    }
}

// Se llama en loop()
void LEDController::update() {

    if (currentMode == DANGER) {

        if (millis() - lastBlink > 300) {
            lastBlink = millis();
            blinkState = !blinkState;

            digitalWrite(redPin, blinkState);
            digitalWrite(yellowPin, blinkState);
            digitalWrite(greenPin, blinkState);
        }
    }
}

Mode LEDController::stringToMode(const String& msg) {

    if (msg == "GREEN") return GREEN;
    if (msg == "YELLOW") return YELLOW;
    if (msg == "RED") return RED;
    if (msg == "DANGER") return DANGER;

    return OFF;
}