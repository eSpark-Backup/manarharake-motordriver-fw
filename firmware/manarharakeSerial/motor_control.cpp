#include "motor_control.hpp"
#include <Arduino.h>

Motor::Motor(int initialSpeed, int initialPosition, int initialSensorSensitivity, int initialPin, ForceSensitiveResistor* fsrPtr) {
    motorSetting.speed = initialSpeed;
    motorSetting.position = initialPosition;
    fsr = fsrPtr;
    fsr->setFsrSenLvl(initialSensorSensitivity);
    motorSetting.pin = initialPin;
    pinMode(initialPin, OUTPUT);
}

void Motor::setSpeed(int newSpeed) {
    motorSetting.speed = newSpeed;
    _setSpeed(motorSetting.speed);
}

void Motor::_setSpeed(int speed) {
    //Motor Control Logic TODO:
    analogWrite(motorSetting.pin, speed);
}

void Motor::setPosition(int newPosition) {
    motorSetting.position = newPosition;
    _setPosition(motorSetting.position);
}

void Motor::_setPosition(int pos) {
    //Motor Position Control logic TODO:
}

void Motor::setSensorSensitivity(int newSensitivity) {
    fsr->setFsrSenLvl(newSensitivity);
}

int Motor::getSpeed() {
    return motorSetting.speed;
}

int Motor::getPosition() {
    return motorSetting.position;
}

int Motor::getSensorSensitivity() {
    return fsr->getFsrSenLvl();
}

void Motor::setPin(int pin)
{
    pinMode(pin, OUTPUT);
}

void Motor::checkFsr()
{
    if(fsr->sensitivityReached())
    {
        Motor::_setSpeed(STOP_SPEED);
    }
}