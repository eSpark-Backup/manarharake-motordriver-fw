#include "motor_control.hpp"
#include <Arduino.h>
#include "board.h"

Motor::Motor(MotorSettings initialMotorSetting, ForceSensitiveResistor* fsrPtr) {
    motorSetting.speed = initialMotorSetting.speed;
    motorSetting.position = initialMotorSetting.position;
    fsr = fsrPtr;
    fsr->setFsrSenLvl(initialMotorSetting.fsrSens);
    motorSetting.pin = initialMotorSetting.pin;
    motorSetting.pin2 = initialMotorSetting.pin2;
    pinMode(motorSetting.pin, OUTPUT);
    pinMode(motorSetting.pin2, OUTPUT);
}

void Motor::setSpeed(int newSpeed) {
    motorSetting.speed = newSpeed;
    _setSpeed(motorSetting.speed);
}

void Motor::_setSpeed(int speed) {
    if(emergencyHalt) return;
    if(motorSetting.direction == CW)
    {
        analogWrite(motorSetting.pin,   speed);
        analogWrite(motorSetting.pin2,  0);
    }
    else if(motorSetting.direction == CCW) {
        analogWrite(motorSetting.pin,  0);
        analogWrite(motorSetting.pin2,   speed);
    }
}

void Motor::setDirection(bool dir) {
    motorSetting.direction = dir;
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
        emergencyHalt = true;
        MH_LOG_LN("EMERGENCY_HALT");
    }
}

int Motor::readCurrentFsrVal()
{
  return fsr->readFsrSens();
}

void Motor::run() {
    _setSpeed(motorSetting.speed);
}

void Motor::stop()
{
    _setSpeed(0);
}