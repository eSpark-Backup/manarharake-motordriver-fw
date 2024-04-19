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
        case 0:
            data->encoder1 = encoderVal;
            break;
        case 1:
            data->encoder2 = encoderVal;
            break;
        case 2:
            data->encoder3 = encoderVal;
            break;
        case 3:
            data->encoder4 = encoderVal;
            break;
        case 4:
            data->encoder5 = encoderVal;
            break;
        case 5:
            data->encoder6 = encoderVal;
            break;
        default:
            std::cerr << "Invalid motor index." << std::endl;
    }
}

void getMotorData(const MotorData* data, int motorIndex, signed int* encoderVal) {
    switch (motorIndex) {
        case 0:
            *encoderVal = data->encoder1;
            break;
        case 1:
            *encoderVal = data->encoder2;
            break;
        case 2:
            *encoderVal = data->encoder3;
            break;
        case 3:
            *encoderVal = data->encoder4;
            break;
        case 4:
            *encoderVal = data->encoder5;
            break;
        case 5:
            *encoderVal = data->encoder6;
            break;
        default:
            std::cerr << "Invalid motor index." << std::endl;
    }
}
