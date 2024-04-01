#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

#include <Arduino.h>

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

//IMPORTANT: Donot Change the command structure
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

struct SerialCommand{
  char* commandString;
  void(*commandCallback)(const String& command);
};


#endif //SERIAL_COMMANDS_H