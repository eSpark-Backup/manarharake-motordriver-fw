#ifndef MANARHARAKE_H
#define MANARHARAKE_H

#include "motor_control.hpp"
#include "fsr.hpp"
#include <Arduino.h>

#define BAUDRATE            9600
#define EEPROM_SIZE_BYTES   100

#define CONFIG_A   'A' 
#define CONFIG_B   'B' 
#define CONFIG_C   'C' 
#define CONFIG_D   'D' 
#define CONFIG_E   'E' 
#define CONFIG_F   'F' 
#define CONFIG_G   'G' 

enum MOTOR_INDEX {
    MOTOR_1 = 0,
    MOTOR_2,
    MOTOR_3,
    MOTOR_4,
    MOTOR_5,
    MOTOR_6,
    MOTOR_MAX = 6
 };

enum DEMO_STATE {
    DEMO_STATE_1,
    DEMO_STATE_2,
    DEMO_STATE_3,
    DEMO_STATE_4,
};

#define SPEED_OFFSET                10
#define POSITION_LOW_OFFSET         20
#define POSITION_HIGH_OFFSET        30
#define SENSOR_SENSITIVITY_OFFSET   40

// #define MOTOR_COUNT sizeof(Manarharake::motor)/sizeof(Manarharake::motor[0])
#define MOTOR_COUNT MOTOR_MAX

class Manarharake{
    private:
        static Motor motor[MOTOR_COUNT];
        static bool demo_state;
    public:
        void initalize();
        void loop();
        static void printConfiguration();
        static void executeArray(char command);
        static void setMotor(const String& command);
        static void setSensor(const String& command);
        static void queryMotor(const String& command);
        static void querySensor(const String& command);
        static void setResistor(const String& command);
        static void loadConfiguration(const String& command);
        static void saveConfiguration(const String& command);
        static void startDemo(const String& command);
        static void stopDemo(const String& command);
        static void checkFsrThreshold();
        static void maintainPos();
};

#endif //MANARHARAKE_H