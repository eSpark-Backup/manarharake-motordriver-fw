#include "rtc_mem.hpp"
#include <string.h>


void loadMotorData(uint32_t* registers, uint32_t* motorData) {
    memcpy(motorData, registers, 16);
}

void saveMotorData(uint32_t* registers, uint32_t* motorData) {
    registers[0] = *motorData;
    registers[1] = *(motorData+1);
    registers[2] = *(motorData+2);
    registers[3] = *(motorData+3);
}

void setMotorData(MotorData* data, int motorIndex, signed int encoderVal) {
    switch (motorIndex) {
        case M0:
            data->encoder1 = encoderVal;
            break;
        case M1:
            data->encoder2 = encoderVal;
            break;
        case M2:
            data->encoder3 = encoderVal;
            break;
        case M3:
            data->encoder4 = encoderVal;
            break;
        case M4:
            data->encoder5 = encoderVal;
            break;
        case M5:
            data->encoder6 = encoderVal;
            break;
        default:
            std::cerr << "Invalid motor index." << std::endl;
    }
}

void getMotorData(const MotorData* data, int motorIndex, signed int* encoderVal) {
    switch (motorIndex) {
        case M0:
            *encoderVal = data->encoder1;
            break;
        case M1:
            *encoderVal = data->encoder2;
            break;
        case M2:
            *encoderVal = data->encoder3;
            break;
        case M3:
            *encoderVal = data->encoder4;
            break;
        case M4:
            *encoderVal = data->encoder5;
            break;
        case M5:
            *encoderVal = data->encoder6;
            break;
        default:
            std::cerr << "Invalid motor index." << std::endl;
    }
}
