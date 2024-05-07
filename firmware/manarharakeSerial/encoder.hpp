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
#define ENCODER1_PINA           GPIO_EMC_05
#define ENCODER1_PINB           GPIO_EMC_06
#define ENCODER1_PINZ           GPIO_EMC_07

#define ENCODER2_PINA           GPIO_EMC_10
#define ENCODER2_PINB           GPIO_EMC_11
#define ENCODER2_PINZ           GPIO_EMC_12

#define ENCODER3_PINA           GPIO_EMC_17
#define ENCODER3_PINB           GPIO_EMC_18
#define ENCODER3_PINZ           GPIO_EMC_19

#define ENCODER4_PINA           GPIO_EMC_22
#define ENCODER4_PINB           GPIO_EMC_23
#define ENCODER4_PINZ           GPIO_EMC_24

#define ENCODER5_PINA           GPIO_EMC_29
#define ENCODER5_PINB           GPIO_EMC_30
#define ENCODER5_PINZ           GPIO_EMC_31

#define ENCODER6_PINA           GPIO_EMC_34
#define ENCODER6_PINB           GPIO_EMC_35
#define ENCODER6_PINZ           GPIO_EMC_36

// #define ENCODER1_PINA           2
// #define ENCODER1_PINB           3

#endif

#define ENC_STEPS_PER_REV       40.0F

/*  2:1 = 0.5.F, 1:10 = 10, 1:2 = 2 means 1 enc rev = 2 motor rev   */
/*  0.5F means 1 encoder rev = 0.5 motor rev*/
#define MOTOR_IN_TO_OUT_RATIO   1.0F 

#define ANGLE_PER_ENC_STEP      (360.0F / ENC_STEPS_PER_REV)
#define MOTOR_STOP_ANGLE           0

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