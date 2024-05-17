#include "motor_control.hpp"
//#include <Arduino.h>
#include "board.h"

Motor::Motor(MotorSettings initialMotorSetting, ForceSensitiveResistor* fsrPtr) {
    motorSetting.speed = initialMotorSetting.speed;
    motorSetting.position = initialMotorSetting.position;
    fsr = fsrPtr;
    fsr->setFsrSenLvl(initialMotorSetting.fsrSens);
    motorSetting.pin = initialMotorSetting.pin;
    motorSetting.pin2 = initialMotorSetting.pin2;
    // pinMode(motorSetting.pin, OUTPUT);
    // pinMode(motorSetting.pin2, OUTPUT);
    // pinMode(motorSetting.motorEnablePin, OUTPUT);
    // digitalWrite(motorSetting.motorEnablePin, HIGH);
}

void Motor::begin(RotaryEncoder *encoderPtr)
{
    pinMode(motorSetting.pin, OUTPUT);
    pinMode(motorSetting.pin2, OUTPUT);
    pinMode(motorSetting.motorEnablePin, OUTPUT);
    digitalWrite(motorSetting.motorEnablePin, HIGH);
    encoder = encoderPtr;
    encoder->begin();
    MH_LOG_LN("Motor Begin");
}

void Motor::setSpeed(int newSpeed) {
    motorSetting.speed = newSpeed;
    _setSpeed(motorSetting.speed);
}

void Motor::_setSpeed(int speed) {
    if(emergencyHalt)
    {
        analogWrite(motorSetting.pin,       STOP_SPEED);
        analogWrite(motorSetting.pin2,      STOP_SPEED);
        return;
    } 

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

    if(getHaltState() == true)
    {
        this->stop();
        return;
    }

    int _currentPos    =   this->getCurrentPostion();
    int _currentAngle  =   this->getMotorAngle();
    int _requestedPos  =   this->getPositionSetting();
    // MH_LOG_LN(_currentPos);
    // MH_LOG_LN(_currentAngle);
    // MH_LOG_LN(_requestedPos);


    if(_currentPos == _requestedPos && _currentAngle == MOTOR_STOP_ANGLE ){ //Rev and angle both matched
      this->stop();
      return;
    }
    else if(_currentPos < _requestedPos || _currentAngle < MOTOR_STOP_ANGLE) //Behind the required position
    {
      this->setDirection(CW);
      this->run();
      return;
    }
    else if(_currentPos > _requestedPos || _currentAngle > MOTOR_STOP_ANGLE){ //Ahead of requried position
      this->setDirection(CCW);
      this->run();
      return;
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
        setHaltState(true);
    }
    else
    {
        setHaltState(false);
    }
}

int Motor::readCurrentFsrVal()
{
  return fsr->readFsrSensMapped();
}

void Motor::run() {
    _setSpeed(RUN_SPEED);
}

void Motor::stop()
{
    _setSpeed(STOP_SPEED);
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



