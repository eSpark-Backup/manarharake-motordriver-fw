#ifndef MOTOR_H
#define MOTOR_H

#include "fsr.hpp"
#include "encoder.hpp"
// Pin definition for ESP32
#define M1_PIN      23
#define M2_PIN      21
#define M3_PIN      18
#define M4_PIN      4
#define M5_PIN      12
#define M6_PIN      27

#define M1_PIN2     22
#define M2_PIN2     19
#define M3_PIN2     5
#define M4_PIN2     2
#define M5_PIN2     13
#define M6_PIN2     14

#define STOP_SPEED              0
#define LOW_SPEED               100
#define HIGH_SPEED              255
#define MAX_POSITION            1000
#define MIN_POSITION            0
#define DEFAULT_SPEED           100
#define DEFAULT_POSITION        0

#define CW                      1
#define CCW                     0
#define MOTOR_DIRECTION         CW
//#define DEFAULT_ANGLE           0 //Angle at which motor stops after doing requested revolutions

// Define MotorSettings structure
struct MotorSettings {
    int position;
    int speed;
    int fsrSens;
    int pin;
    int pin2;
    bool direction = MOTOR_DIRECTION;
};

// Define MotorsConfig structure
struct MotorsConfig {
    MotorSettings   array_motors[6];
};

class Motor {
private:
    MotorSettings   motorSetting;
    ForceSensitiveResistor *fsr;
    RotaryEncoder *encoder;
    int currentPosition;
    bool emergencyHalt = false;
    int angle;
    int encoderRotations;
public:
    Motor(MotorSettings initalMotorSetting, ForceSensitiveResistor* fsrPtr, RotaryEncoder *encoderPtr);

    void begin();
    void setPin(int pin);
    void setSpeed(int newSpeed);
    void _setSpeed(int speed);
    void setPosition(int newPosition);
    void maintainPosition();
    void setDirection(bool dir);
    int  readCurrentFsrVal();
    void setSensorSensitivity(int newSensitivity);
    int  getSensorSensitivity();
    int  getSpeedSetting();
    int  getPositionSetting();
    int  getCurrentPostion();
    void run();
    void stop();
    void checkFsr();
    void setEncoder(int position){
        encoder->setEncoder(position);
    }
    long getEncoder(){
        return encoder->getPosition();
    }

    int getMotorAngle();
};

#define MOTOR_ARRAY_CONF_COUNT  7
// Array to store motor positions and speeds for each array letter (orignal required configs)
const MotorsConfig motorConfiguration[MOTOR_ARRAY_CONF_COUNT] = {
    {{{700, DEFAULT_SPEED}, {500,   DEFAULT_SPEED}, {700,   DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}}},   // A
    {{{700, DEFAULT_SPEED}, {1000,  DEFAULT_SPEED}, {700,   DEFAULT_SPEED}, {700,   DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}}},   // B
    {{{900, DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}}},   // C
    {{{0,   DEFAULT_SPEED}, {500,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}}},   // D
    {{{800, DEFAULT_SPEED}, {500,   DEFAULT_SPEED}, {500,   DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}}},   // E
    {{{700, DEFAULT_SPEED}, {700,   DEFAULT_SPEED}, {850,   DEFAULT_SPEED}, {850,   DEFAULT_SPEED}, {850,   DEFAULT_SPEED}, {850,   DEFAULT_SPEED}}},   // F
    {{{0,   DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}}}    // G
};

// Array to store motor positions and speeds for each array letter (testing)
// const MotorsConfig motorConfiguration[MOTOR_ARRAY_CONF_COUNT] = {
//     {{{700,   0   },    {500,   50  }, {700,  100 }, {0,    150 }, {0,    200 }, {0,    255 }}},    // A
//     {{{700,   150 },    {1000,  0   }, {700,  255 }, {700,  150 }, {0,    255 }, {0,    255 }}},    // B
//     {{{900,   255 },    {900,   255 }, {900,  0   }, {900,  255 }, {900,  255 }, {900,  255 }}},    // C
//     {{{0,     255 },    {500,   150 }, {900,  180 }, {900,  0   }, {900,  255 }, {900,  255 }}},    // D
//     {{{800,   180 },    {500,   255 }, {500,  255 }, {0,    150 }, {0,    0   }, {0,    255 }}},    // E
//     {{{700,   255 },    {700,   255 }, {850,  255 }, {850,  255 }, {850,  180 }, {850,  0   }}},    // F
//     {{{0,     0   },    {0,     50  }, {0,    50  }, {0,    0   }, {0,    50  }, {0,    0   }}}     // G
// };

#endif