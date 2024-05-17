#include "encoder.hpp"
#include "manarharake.hpp"

std::vector<RotaryEncoder> encoders = {
    RotaryEncoder(ENCODER1_PINA, ENCODER1_PINB),
    RotaryEncoder(ENCODER2_PINA, ENCODER2_PINB),
    RotaryEncoder(ENCODER3_PINA, ENCODER3_PINB),
    RotaryEncoder(ENCODER4_PINA, ENCODER4_PINB),
    RotaryEncoder(ENCODER5_PINA, ENCODER5_PINB),
    RotaryEncoder(ENCODER6_PINA, ENCODER6_PINB),
};

CREATE_GLOBAL_ENCODER(0);
CREATE_GLOBAL_ENCODER(1);
CREATE_GLOBAL_ENCODER(2);
CREATE_GLOBAL_ENCODER(3);
CREATE_GLOBAL_ENCODER(4);
CREATE_GLOBAL_ENCODER(5);

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

void RotaryEncoder::setup() {

    encoders[0].isrCallback = _isrCallback_0;
    encoders[1].isrCallback = _isrCallback_1;
    encoders[2].isrCallback = _isrCallback_2;
    encoders[3].isrCallback = _isrCallback_3;
    encoders[4].isrCallback = _isrCallback_4;
    encoders[5].isrCallback = _isrCallback_5;
}

std::vector<RotaryEncoder>& RotaryEncoder::getEncoders() {
    static bool condition = false;
    if(!condition)
    {
        setup();
        condition = true;

    }
    return encoders;
}