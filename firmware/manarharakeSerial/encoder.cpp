#include "encoder.hpp"


void RotaryEncoder::begin() {
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    currentStateA = digitalRead(pinA);
    currentStateB = digitalRead(pinB);
    previousStateA = currentStateA;
    previousStateB = currentStateB;
    attachInterrupt(digitalPinToInterrupt(pinA), isrCallback, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pinB), isrCallback, CHANGE);
    Serial.println("Encoder Begin!");
}

long RotaryEncoder::getPosition() {
    return currentPosition;
}