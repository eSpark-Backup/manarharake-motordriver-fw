#ifndef RTC_MEM_H
#define RTC_MEM_H
#include <iostream>

// Define constants
#define BITS_PER_ANGLE      9 // Assuming 9 bits for angle
#define BITS_PER_ROTATION   12 // Assuming 12 bits for rotation
#define BITS_PER_ENCODER    21

enum motorIndex_t {
    M0,
    M1,
    M2,
    M3,
    M4,
    M5,
};

// // Define a struct to represent motor data
// struct MotorData {
//     signed int angle1 : BITS_PER_ANGLE;
//     signed int rotation1 : BITS_PER_ROTATION;
//     signed int angle2 : BITS_PER_ANGLE;
//     signed int rotation2 : BITS_PER_ROTATION;
//     signed int angle3 : BITS_PER_ANGLE;
//     signed int rotation3 : BITS_PER_ROTATION;
//     signed int angle4 : BITS_PER_ANGLE;
//     signed int rotation4 : BITS_PER_ROTATION;
//     signed int angle5 : BITS_PER_ANGLE;
//     signed int rotation5 : BITS_PER_ROTATION;
//     signed int angle6 : BITS_PER_ANGLE;
//     signed int rotation6 : BITS_PER_ROTATION;
// } __attribute__((packed)); // This attribute ensures that the struct is packed tightly


// Define a struct to represent motor data
struct MotorData {
    signed int encoder1 : BITS_PER_ENCODER;
    signed int encoder2 : BITS_PER_ENCODER;
    signed int encoder3 : BITS_PER_ENCODER;
    signed int encoder4 : BITS_PER_ENCODER;
    signed int encoder5 : BITS_PER_ENCODER;
    signed int encoder6 : BITS_PER_ENCODER;
} __attribute__((packed)); // This attribute ensures that the struct is packed tightly


/* Populate Custom 128 bit MotorData strucuture */
void setMotorData(MotorData* data, int motorIndex, signed int encoderVal);
/* Get motor data from custom structure to normal struct for faster operation*/
void getMotorData(const MotorData* data, int motorIndex, signed int* encoderVal);
/* Save MotorData in to RTC mem 4x32bit registers*/
void saveMotorData(uint32_t* registers, uint32_t* motorData);
/* Load MotorData from RTC registers */
void loadMotorData(uint32_t* registers, uint32_t* motorData);

#endif //RTC_MEM_H