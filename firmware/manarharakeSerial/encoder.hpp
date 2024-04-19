#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <map>

#define ENCODER1_PINA           25
#define ENCODER1_PINB           26

#define ENC_STEPS_PER_REV       40.0F

/*  2:1 = 0.5.F, 1:10 = 10, 1:2 = 2 means 1 enc rev = 2 motor rev   */
/*  0.5F means 1 encoder rev = 0.5 motor rev*/
#define MOTOR_IN_TO_OUT_RATIO   1.0F 

#define ANGLE_PER_ENC_STEP      (360.0F / ENC_STEPS_PER_REV)
#define DEFAULT_ANGLE           0

class RotaryEncoder {
private:
    int pinA;
    int pinB;
    volatile int currentStateA;
    volatile int currentStateB;
    volatile int previousStateA;
    volatile int previousStateB;
    volatile long currentPosition = 0;
    static std::map<int, RotaryEncoder*> instances;

public:
    RotaryEncoder(int pinA, int pinB) : pinA(pinA), pinB(pinB) {}

    void begin();

    long getPosition();

    void setEncoder(int pos)
    {
        currentPosition = pos;
    }
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