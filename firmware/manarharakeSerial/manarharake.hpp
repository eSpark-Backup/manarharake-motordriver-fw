#ifndef MANARHARAKE_H
#define MANARHARAKE_H

#include "motor_control.hpp"
#include "fsr.hpp"
//#include <Arduino.h>

#define BAUDRATE            9600
#define EEPROM_SIZE_BYTES   100

#define CONFIG_A   'A' 
#define CONFIG_B   'B' 
#define CONFIG_C   'C' 
#define CONFIG_D   'D' 
#define CONFIG_E   'E' 
#define CONFIG_F   'F' 
#define CONFIG_G   'G' 

#define DEFAULT_ARRAY_CONFIG    CONFIG_A

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
// #define MOTOR_COUNT 2

#define SELECTOR_RESISTOR   1
#define SCROLL_RESISTOR     2

#define HYSTERISIS          100

#define DEFAULT_ACTIVATION 512
#define DEFAULT_ACTIVATION_R1   DEFAULT_ACTIVATION
#define DEFAULT_ACTIVATION_R2   DEFAULT_ACTIVATION

#define R1      arraySelectionPot
#define R2      arrayScrollPot

class Potentiometer{
    private:
        int activationThreshold;
        int currentValue;
        int pin;
        bool incremented = false;
        int deactivationThreshold;
    public:
        bool toggle = false;
        Potentiometer(int _pin, int _activation)
        {
            pin = _pin;
            activationThreshold = _activation;
            deactivationThreshold =  activationThreshold - HYSTERISIS;
        }
        void initalize(){
            pinMode(pin, INPUT);
            MH_LOG_LN("POT Begin");
        }
        int getvalue(){
            return currentValue = analogRead(pin);
        }
        bool reachedActivation()
        {
            return (getvalue() >= activationThreshold) ? true : false;
        }
        bool reachedDeactivation()
        {
            return (getvalue() <= deactivationThreshold) ? true : false;
        }
        void resetFlag()
        {
            incremented = false;
        }
        void setFlag()
        {
            incremented = true;
        }
        bool incrementFlag()
        {
            return incremented;
        }
        void setActivation(int _activation)
        {
            activationThreshold = _activation;
            deactivationThreshold =  activationThreshold - HYSTERISIS;
            MH_LOG_LN("Activation Apllied");
        }
};

class Manarharake{
    private:
        static Motor motor[MOTOR_COUNT];
        static char currenConfigArray;
        static bool demo_state;
        static Potentiometer arrayScrollPot, arraySelectionPot;
        static int counter;  //config count while demo or pot selection increment

    public:
        void initalize();
        static void loop();
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
        static void potentiometerTask();
        static void setResisitorActivation(int resNum, int activation);
};

#endif //MANARHARAKE_H