#ifndef MOTOR_H
#define MOTOR_H

// Pin definition for ESP32
#define M1_PIN  23
#define M2_PIN  22
#define M3_PIN  21
#define M4_PIN  19
#define M5_PIN  5
#define M6_PIN  16

class Motor {
private:
    int speed;
    int position;
    int sensorSensitivity;
    int pin;

public:
    Motor(int initialSpeed, int initialPosition, int initialSensorSensitivity, int pin);

    void setSpeed(int newSpeed);
    void setPosition(int newPosition);
    void setSensorSensitivity(int newSensitivity);
    void setPin(int pin);
    int getSpeed();
    int getPosition();
    int getSensorSensitivity();
};

// Define MotorSettings structure
struct MotorSettings {
    int position;
    int speed;
};

// Define MotorsConfig structure
struct MotorsConfig {
    MotorSettings array_motors[6];
};

// // Array to store motor positions and speeds for each array letter
// const MotorsConfig motorConfiguration[7] = {
//     {{{700, 255}, {500, 255}, {700, 255}, {0, 255}, {0, 255}, {0, 255}}},   // A
//     {{{700, 255}, {1000, 255}, {700, 255}, {700, 255}, {0, 255}, {0, 255}}}, // B
//     {{{900, 255}, {900, 255}, {900, 255}, {900, 255}, {900, 255}, {900, 255}}}, // C
//     {{{0, 255}, {500, 255}, {900, 255}, {900, 255}, {900, 255}, {900, 255}}}, // D
//     {{{800, 255}, {500, 255}, {500, 255}, {0, 255}, {0, 255}, {0, 255}}},     // E
//     {{{700, 255}, {700, 255}, {850, 255}, {850, 255}, {850, 255}, {850, 255}}}, // F
//     {{{0, 255}, {0, 255}, {0, 255}, {0, 255}, {0, 255}, {0, 255}}}             // G
// };


// Array to store motor positions and speeds for each array letter
const MotorsConfig motorConfiguration[7] = {
    {{{700,   0   },    {500,   50  }, {700,  100 }, {0,    150 }, {0,    200 }, {0,    255 }}},    // A
    {{{700,   150 },    {1000,  0   }, {700,  255 }, {700,  150 }, {0,    255 }, {0,    255 }}},    // B
    {{{900,   255 },    {900,   255 }, {900,  0   }, {900,  255 }, {900,  255 }, {900,  255 }}},    // C
    {{{0,     255 },    {500,   150 }, {900,  180 }, {900,  0   }, {900,  255 }, {900,  255 }}},    // D
    {{{800,   180 },    {500,   255 }, {500,  255 }, {0,    150 }, {0,    0   }, {0,    255 }}},    // E
    {{{700,   255 },    {700,   255 }, {850,  255 }, {850,  255 }, {850,  180 }, {850,  0   }}},    // F
    {{{0,     0   },    {0,     50  }, {0,    50  }, {0,    0   }, {0,    50  }, {0,    0   }}}     // G
};

#endif