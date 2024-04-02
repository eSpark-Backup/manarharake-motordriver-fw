#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <map>

#define ENCODER1_PINA           13
#define ENCODER1_PINB           14

#define ENC_STEPS_PER_REV       1.0F
#define MOTOR_IN_TO_OUT_RATIO   1.0F //2:1 = 0.5.F, 1:10 = 10

class RotaryEncoder {
private:
    int pinA;
    int pinB;
    volatile int currentStateA;
    volatile int currentStateB;
    volatile int previousStateA;
    volatile int previousStateB;
    volatile int16_t currentPosition;
    static std::map<int, RotaryEncoder*> instances;

public:
    RotaryEncoder(int pinA, int pinB) : pinA(pinA), pinB(pinB) {}

    void begin();

    int16_t getPosition();

private:
    static void handleEncoder() {
        for (auto& pair : instances) {
            if (pair.first == digitalPinToInterrupt(pair.second->pinA) || 
                pair.first == digitalPinToInterrupt(pair.second->pinB)) {
                pair.second->updatePosition();
            }
        }
    }

    void updatePosition();
};


#endif //ENCODER_H