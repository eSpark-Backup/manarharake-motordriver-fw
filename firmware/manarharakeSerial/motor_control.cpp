#include "motor_control.hpp"
#include <Arduino.h>

Motor::Motor(int initialSpeed, int initialPosition, int initialSensorSensitivity, int initialPin) {
    speed = initialSpeed;
    position = initialPosition;
    sensorSensitivity = initialSensorSensitivity;
    pin = initialPin;
    pinMode(initialPin, OUTPUT);
}

void Motor::setSpeed(int newSpeed) {
    analogWrite(pin, newSpeed);
    speed = newSpeed;
}

void Motor::setPosition(int newPosition) {
    position = newPosition;
}

void Motor::setSensorSensitivity(int newSensitivity) {
    sensorSensitivity = newSensitivity;
}

int Motor::getSpeed() {
    return speed;
}

int Motor::getPosition() {
    return position;
}

int Motor::getSensorSensitivity() {
    return sensorSensitivity;
}
