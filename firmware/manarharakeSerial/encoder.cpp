#include "encoder.hpp"

std::map<int, RotaryEncoder*> RotaryEncoder::instances;

void RotaryEncoder::begin() {
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    currentStateA = digitalRead(pinA);
    currentStateB = digitalRead(pinB);
    previousStateA = currentStateA;
    previousStateB = currentStateB;
    currentPosition = 0;
    instances[digitalPinToInterrupt(pinA)] = this;
    instances[digitalPinToInterrupt(pinB)] = this;
    attachInterrupt(digitalPinToInterrupt(pinA), handleEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pinB), handleEncoder, CHANGE);
    Serial.println("Encoder Begin!");
}

int16_t RotaryEncoder::getPosition() {
    return currentPosition;
}

void RotaryEncoder::updatePosition() {
    currentStateA = digitalRead(pinA);
    currentStateB = digitalRead(pinB);

    if (currentStateA != previousStateA) {
        if (currentStateA != currentStateB) {
            currentPosition++;
        } else {
            currentPosition--;
        }
    }
    previousStateA = currentStateA;
    previousStateB = currentStateB;
}