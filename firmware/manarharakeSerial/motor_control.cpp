#include "motor_control.hpp"
#include <Arduino.h>
#include "board.h"

Motor::Motor(MotorSettings initialMotorSetting, ForceSensitiveResistor* fsrPtr, RotaryEncoder *encoderPtr) {
    motorSetting.speed = initialMotorSetting.speed;
    motorSetting.position = initialMotorSetting.position;
    fsr = fsrPtr;
    encoder = encoderPtr;
    fsr->setFsrSenLvl(initialMotorSetting.fsrSens);
    motorSetting.pin = initialMotorSetting.pin;
    motorSetting.pin2 = initialMotorSetting.pin2;
    pinMode(motorSetting.pin, OUTPUT);
    pinMode(motorSetting.pin2, OUTPUT);
}

void Motor::begin()
{
    encoder->begin();
}

void Motor::setSpeed(int newSpeed) {
    motorSetting.speed = newSpeed;
    _setSpeed(motorSetting.speed);
}

void Motor::_setSpeed(int speed) {
    if(emergencyHalt) return;
    if(motorSetting.direction == CW)
    {
        analogWrite(motorSetting.pin,       speed);
        analogWrite(motorSetting.pin2,      LOW);
    }
    else if(motorSetting.direction == CCW) {
        analogWrite(motorSetting.pin,       LOW);
        analogWrite(motorSetting.pin2,      speed);
    }
}

void Motor::setDirection(bool dir) {
    motorSetting.direction = dir;
}

void Motor::setPosition(int newPosition) {
    motorSetting.position = newPosition;
}

void Motor::maintainPosition() {
    //Motor Position Control logic TODO:

    int _currentPos    =   this->getCurrentPostion();
    int _currentAngle  =   this->getMotorAngle();
    int _requestedPos  =   this->getPositionSetting();

    if(_currentPos < _requestedPos) //Behind the required position
    {
      this->setDirection(CW);
      this->run();
    }
    else if(_currentPos > _requestedPos){ //Ahead of requried position
      this->setDirection(CCW);
      this->run();
    }
    else if(_currentPos == _requestedPos && _currentAngle == DEFAULT_ANGLE){ //Rev and angle both matched
      this->stop();
    }
    else if(_currentAngle > DEFAULT_ANGLE){ //Angle > requried Angle
      this->setDirection(CCW);
      this->run();
    }
    else if(_currentAngle < DEFAULT_ANGLE){ //Angle < requried Angle
      this->setDirection(CW);
      this->run();
    }
}

void Motor::setSensorSensitivity(int newSensitivity) {
    fsr->setFsrSenLvl(newSensitivity);
}

int Motor::getSpeedSetting() {
    return motorSetting.speed;
}

int Motor::getPositionSetting() {
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
        setHaltState(true);
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

int Motor::getCurrentPostion()
{
    /* Calculate with steps per rev of encoder and gear ratio, 
    40 steps of encoder with 1:1 gear ratio = 1 turn*/

    /*  Get encoder raw value   */
    long _encoderVal = encoder->getPosition();

    /*  Motor Position using gear ratio if used*/
    currentPosition = (_encoderVal/ENC_STEPS_PER_REV)*MOTOR_IN_TO_OUT_RATIO;

    /* Angle calculation from encoder steps*/
    angle = ((int)(_encoderVal/ENC_STEPS_PER_REV*360*MOTOR_IN_TO_OUT_RATIO))%360;
    
    /* Encoder Rotations not compensating gear ratio*/
    encoderRotations = encoder->getPosition()/ENC_STEPS_PER_REV;

    return currentPosition;

    /* FORMULA USED >>>>>>>
    currentPos = (encoder_val/enc_res)*Ratio;
    angle = ((encoder_val/enc_res)*360*ratio)%360 

    e,g;
    encoder count = 620
    620/40 = 15.5
    15.5 * 360 = 5580
    5580 % 360 = 180

    so, revs = 15, angle = 180

    */
}

int Motor::getMotorAngle()
{
    return angle;
}



