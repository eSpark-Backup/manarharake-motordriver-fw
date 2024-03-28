#ifndef MANARHARAKE_H
#define MANARHARAKE_H

#include "motor_control.hpp"
#include "fsr.hpp"
#include <Arduino.h>

#define CONFIG_A   'A' 
#define CONFIG_B   'B' 
#define CONFIG_C   'C' 
#define CONFIG_D   'D' 
#define CONFIG_E   'E' 
#define CONFIG_F   'F' 
#define CONFIG_G   'G' 

enum MOTOR_INDEX {
    MOTOR_1 = 1,
    MOTOR_2,
    MOTOR_3,
    MOTOR_4,
    MOTOR_5,
    MOTOR_6,
    MOTOR_MAX = 6
 };

#define SPEED_OFFSET 10
#define POSITION_LOW_OFFSET 20
#define POSITION_HIGH_OFFSET 30
#define SENSOR_SENSITIVITY_OFFSET 40

// #define MOTOR_COUNT sizeof(Manarharake::motor)/sizeof(Manarharake::motor[0])
#define MOTOR_COUNT MOTOR_MAX

/* Current Commands */
#define SET_COM         "SET"
#define QUERY_COM       "QUERY"
#define POSITION_COM    "POSITION"
#define SPEED_COM       "SPEED"
#define SENSITIVITY_COM "SENSITIVITY"
#define MOTOR_COM       "MOTOR"
#define SENSOR_COM      "SENSOR"

/* COMMAND STRUCTURE */
/* COMMAND ITEM (itemIndex) PARAMETER (param_value) */
//Command
#define COMMAND_TYPE_1  SET_COM
#define COMMAND_TYPE_2  QUERY_COM
//Items
#define ITEM_1          MOTOR_COM            
#define ITEM_2          SENSOR_COM
//Parameters
#define PARAM1          SPEED_COM
#define PARAM2          POSITION_COM
#define PARAM3          SENSITIVITY_COM          

//NOTE: Donot Change the command structure
#define SET_MOTOR_CMD       COMMAND_TYPE_1 " " ITEM_1
#define SET_SENSOR_CMD      COMMAND_TYPE_1 " " ITEM_2
#define QUERY_MOTOR_CMD     COMMAND_TYPE_2 " " ITEM_1
#define QUERY_SENSOR_CMD    COMMAND_TYPE_2 " " ITEM_2
#define SET_RESISTANCE_CMD  "SET R"
#define START_DEMO_CMD      "START DEMO"
#define STOP_DEMO_CMD       "STOP DEMO"
#define LOAD_CONFIG_CMD     "LOAD CONFIG"
#define SAVE_CONFIG_CMD     "SAVE CONFIG"

#define COMMAND_ADD_PARAM(x) \
    if (command.indexOf(x) != -1) { \
        queryType = x; \
        value = command.substring(command.indexOf(x) + strlen(x)).toInt(); \
    }

#define DEFAULT_SENSITIVITY 1


class Manarharake{
    private:
        static Motor motor[6];
    public:
        void initalize();
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
};

#endif //MANARHARAKE_H