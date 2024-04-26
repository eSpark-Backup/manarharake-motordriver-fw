#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <map>
#include "board.h"
#if(BOARD == ESP32_BOARD)
#define ENCODER1_PINA           25
#define ENCODER1_PINB           26
#endif

#if(BOARD == TEENSY_BOARD)
#define ENCODER1_PINA           3
#define ENCODER1_PINB           4

#define ENCODER2_PINA           58
#define ENCODER2_PINB           59

#define ENCODER3_PINA           65
#define ENCODER3_PINB           66

#define ENCODER4_PINA           70
#define ENCODER4_PINB           71

#define ENCODER5_PINA           78
#define ENCODER5_PINB           79

#define ENCODER6_PINA           81
#define ENCODER6_PINB           82
#endif

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