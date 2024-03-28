#ifndef MOTOR_H
#define MOTOR_H

#include "fsr.hpp"

// Pin definition for ESP32
#define M1_PIN  23
#define M2_PIN  22
#define M3_PIN  21
#define M4_PIN  19
#define M5_PIN  5
#define M6_PIN  16

#define STOP_SPEED              0
#define LOW_SPEED               100
#define HIGH_SPEED              255
#define MAX_POSITION            1000
#define MIN_POSITION            0
#define DEFAULT_SPEED           255
#define DEFAULT_POSITION        0

// Define MotorSettings structure
struct MotorSettings {
    int position;
    int speed;
    int fsrSens;
    int pin;
};

struct FsrSettings {
    int sensitivity;
};

// Define MotorsConfig structure
struct MotorsConfig {
    MotorSettings   array_motors[6];
};

class Motor {
private:
    MotorSettings   motorSetting;
public:
    ForceSensitiveResistor *fsr;
    Motor(int initialSpeed, int initialPosition, int initialSensorSensitivity, int pin, ForceSensitiveResistor* fsrPtr);

    void setSpeed(int newSpeed);
    void _setSpeed(int speed);
    void setPosition(int newPosition);
    void _setPosition(int pos);
    void setSensorSensitivity(int newSensitivity);
    void setPin(int pin);
    int getSpeed();
    int getPosition();
    int getSensorSensitivity();
    void checkFsr();
};

#define MOTOR_ARRAY_CONF_COUNT  7
// Array to store motor positions and speeds for each array letter
const MotorsConfig motorConfiguration[MOTOR_ARRAY_CONF_COUNT] = {
    {{{700, DEFAULT_SPEED}, {500,   DEFAULT_SPEED}, {700,   DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}}},   // A
    {{{700, DEFAULT_SPEED}, {1000,  DEFAULT_SPEED}, {700,   DEFAULT_SPEED}, {700,   DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}}},   // B
    {{{900, DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}}},   // C
    {{{0,   DEFAULT_SPEED}, {500,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}, {900,   DEFAULT_SPEED}}},   // D
    {{{800, DEFAULT_SPEED}, {500,   DEFAULT_SPEED}, {500,   DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}}},   // E
    {{{700, DEFAULT_SPEED}, {700,   DEFAULT_SPEED}, {850,   DEFAULT_SPEED}, {850,   DEFAULT_SPEED}, {850,   DEFAULT_SPEED}, {850,   DEFAULT_SPEED}}},   // F
    {{{0,   DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}, {0,     DEFAULT_SPEED}}}    // G
};

// Array to store motor positions and speeds for each array letter
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