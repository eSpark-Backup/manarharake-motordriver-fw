#include "fsr.hpp"
#include <Arduino.h>


ForceSensitiveResistor::ForceSensitiveResistor(int pin) {
    setFsrPin(pin);
}

int ForceSensitiveResistor::getFsrSenLvl(){
    return sensitivtyLevel;
}
void ForceSensitiveResistor::setFsrSenLvl(int value){
    
    sensitivtyLevel = value;
    if(value > MAX_FSR_SENS)
    {
        sensitivtyLevel = MAX_FSR_SENS;
    }
    else if(value < MIN_FSR_SENS)
    {
        sensitivtyLevel = MIN_FSR_SENS;
    }
    sensitivityRaw = map(sensitivtyLevel, MIN_FSR_SENS, MAX_FSR_SENS, 0, 4095);
}
int ForceSensitiveResistor::readFsrSens(){
    currentSensitivity = analogRead(fsrPin);
    return currentSensitivity;
}

void ForceSensitiveResistor::setFsrPin(int pin) {
    fsrPin = pin;
    pinMode(fsrPin, INPUT);
}

bool ForceSensitiveResistor::sensitivityReached() {
    ForceSensitiveResistor::readFsrSens();
    if(currentSensitivity >= sensitivityRaw)
    {
        return true;
    }
    else
    {
        return false;
    }
}