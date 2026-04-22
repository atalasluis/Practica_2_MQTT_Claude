#pragma once
#include <Arduino.h>

enum Mode {
    OFF,
    GREEN,
    YELLOW,
    RED,
    DANGER
};

class LEDController {
private:
    int redPin;
    int yellowPin;
    int greenPin;

    Mode currentMode;
    unsigned long lastBlink;
    bool blinkState;

public:
    LEDController(int r, int y, int g);

    void init();
    void setMode(Mode mode);
    void update(); // para parpadeo

    static Mode stringToMode(const String& msg);
};