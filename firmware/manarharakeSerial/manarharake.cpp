#include "manarharake.hpp"
#include "board.h"
#if (BOARD == ARDUINO_BOARD)
#include <Arduino_FreeRTOS.h>
#endif
#if (BOARD == TEENSY_BOARD)
#include "arduino_freertos.h"
#endif
#if  (BOARD == ESP32_BOARD)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#include <Arduino.h>
#include <EEPROM.h>
#include "motor_control.hpp"
#include "serial_commands.hpp"

/* Prototypes */
static void printTask(void *pvParameters);
static void demoTask(void *pvParameters);
static void mainTask(void *pvParameters);

// Global variables
TaskHandle_t demoTaskHandle = NULL;  // Handle for the blinking task
TaskHandle_t mainTaskHandle = NULL;   // Handle for the blinking task
TaskHandle_t printTaskHandle = NULL;   // Handle for the blinking task

// Definition of static member variable
Motor Manarharake::motor[6] = {
    Motor(100, 0, 5, M1_PIN), // Motor 1
    Motor(100, 0, 5, M2_PIN), // Motor 2
    Motor(100, 0, 5, M3_PIN), // Motor 3
    Motor(100, 0, 5, M4_PIN), // Motor 4
    Motor(100, 0, 5, M5_PIN), // Motor 5
    Motor(100, 0, 5, M6_PIN)  // Motor 6
};

SerialCommand commands[] = {
  {SET_MOTOR_CMD      , Manarharake::setMotor          },
  {SET_SENSOR_CMD     , Manarharake::setSensor         },
  {QUERY_MOTOR_CMD    , Manarharake::queryMotor        },
  {QUERY_SENSOR_CMD   , Manarharake::querySensor       },
  {SET_RESISTANCE_CMD , Manarharake::setResistor       },
  {START_DEMO_CMD     , Manarharake::startDemo         },
  {STOP_DEMO_CMD      , Manarharake::stopDemo          },
  {SAVE_CONFIG_CMD    , Manarharake::saveConfiguration },
  {LOAD_CONFIG_CMD    , Manarharake::loadConfiguration }
};

void Manarharake::initalize()
{
    Serial.begin(115200);
    EEPROM.begin(100);

    // Create Tasks
    xTaskCreate(printTask, "printTask", 2*2048, NULL, 0, &printTaskHandle);
    xTaskCreate(mainTask, "mainTask", 2*2048, NULL, 1, &mainTaskHandle);
    xTaskCreate(demoTask, "BlinkLED", 2*2048, NULL, 2, &demoTaskHandle);
    vTaskSuspend(demoTaskHandle);
    // vTaskStartScheduler();

    loadConfiguration("NULL");
    Serial.println("Controller Started");
}



static void mainTask(void *pvParameters) {
  
  for(;;) {
    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n');  // Read the command until newline character
      for(int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++)
      {
        if(command.startsWith(commands[i].commandString))
        {
          commands[i].commandCallback(command);
          command = "";
          break;
        }
      }
      if (command.charAt(0) >= CONFIG_A && command.charAt(0) <= CONFIG_G) {
          Manarharake::executeArray(command.charAt(0));  // Call executeArray function for array commands
      }
    }
    vTaskDelay(10);
  }
}

// Task to execute predefined config array
static void demoTask(void *pvParameters) 
{
  (void)pvParameters;

  char demoArray;
  for(;;) {
    for (int i = 0; i < 6; i++) {
      demoArray = char(CONFIG_A + i);
      Manarharake::executeArray(demoArray); // Execute arrays A-F sequentially
      vTaskDelay(1000/portTICK_PERIOD_MS); // Delay 1 second
      
      demoArray = CONFIG_G;
      Manarharake::executeArray(demoArray); // Execute array G after arrays A-F
      vTaskDelay(1000/portTICK_PERIOD_MS); // Delay 1 second
    }
  }
}

static void printTask(void *pvParameters) 
{
  (void) pvParameters;
  for(;;)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Manarharake::printConfiguration();
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void Manarharake::printConfiguration() 
{
  // Print total configuration for all 6 motors and sensors
  Serial.println("Total Configuration:");

  for (int i = 0; i < MOTOR_COUNT; i++) {
    Serial.print(i + 1);
    Serial.print(": Speed=");
    Serial.print(motor[i].getSpeed());
    Serial.print(", Pos=");
    Serial.print(motor[i].getPosition());
    Serial.print(", Sensor ");
    Serial.print(i + 1);
    Serial.print(": Sens=");
    Serial.println(motor[i].getSensorSensitivity());
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void Manarharake::executeArray(char command) 
{
  char arrayLetter = command;
  if (arrayLetter < CONFIG_A || arrayLetter > CONFIG_G) {
    Serial.println("Invalid array letter.");
    return; // Exit the function if the array letter is invalid
  }
  
  int arrayIndex = arrayLetter - CONFIG_A; // Calculate array index
  
  // Execute movements for each motor
  for (int i = 0; i < MOTOR_COUNT; i++) {


    int position = motorConfiguration[arrayIndex].array_motors[i].position; // Get position value for motor i
    int speed = motorConfiguration[arrayIndex].array_motors[i].speed; // Get speed value for motor i

    // Adjust motor number according to the changed numbering (m6, m5, m1, m2, m3, m4)
    int motorNumber;
    switch(i) {
      case 0: motorNumber = MOTOR_6; break;
      case 1: motorNumber = MOTOR_5; break;
      case 2: motorNumber = MOTOR_1; break;
      case 3: motorNumber = MOTOR_2; break;
      case 4: motorNumber = MOTOR_3; break;
      case 5: motorNumber = MOTOR_4; break;
    }
    
    motor[motorNumber - 1].setSpeed(speed); // Set speed for motor i
    motor[motorNumber - 1].setPosition(position); // Set speed for motor i
  }
}

void Manarharake::loadConfiguration(const String& command)
{
  for(int i = 0; i < MOTOR_COUNT; i++) {
      int speedAddress = SPEED_OFFSET + i;
      int positionLowAddress = POSITION_LOW_OFFSET + i;
      int positionHighAddress = POSITION_HIGH_OFFSET + i;
      int sensorSensitivityAddress = SENSOR_SENSITIVITY_OFFSET + i;

      int speed = EEPROM.read(speedAddress);
      int positionLow = EEPROM.read(positionLowAddress);
      int positionHigh = EEPROM.read(positionHighAddress);
      int sensorSensitivity = EEPROM.read(sensorSensitivityAddress);

      motor[i].setSpeed(speed);
      motor[i].setPosition(positionLow | (positionHigh << 8));
      motor[i].setSensorSensitivity(sensorSensitivity);
  }
}

void Manarharake::saveConfiguration(const String& command)
{
  for(int i = 0; i < MOTOR_COUNT; i++) {
      int speedAddress = SPEED_OFFSET + i;
      int positionLowAddress = POSITION_LOW_OFFSET + i;
      int positionHighAddress = POSITION_HIGH_OFFSET + i;
      int sensorSensitivityAddress = SENSOR_SENSITIVITY_OFFSET + i;

      int speed = motor[i].getSpeed();
      int position = motor[i].getPosition();
      int sensorSensitivity = motor[i].getSensorSensitivity();

      EEPROM.write(speedAddress, speed);
      EEPROM.write(positionLowAddress, position & 0xFF);  // Low byte
      EEPROM.write(positionHighAddress, (position >> 8) & 0xFF);  // High byte
      EEPROM.write(sensorSensitivityAddress, sensorSensitivity);
  }
  EEPROM.commit();
}

void Manarharake::stopDemo(const String& command)
{
  // Delete the blinking task if it exists
  if (demoTaskHandle != NULL) {
    vTaskResume(printTaskHandle);
    vTaskSuspend(demoTaskHandle);
    loadConfiguration("NULL");
  }
}

void Manarharake::startDemo(const String& command)
{
  vTaskSuspend(printTaskHandle);
  vTaskResume(demoTaskHandle);
}

void parseCommand(const String& command, int& motorNum, String& queryType, String queryFor, int* val = NULL, int* motorIndex = NULL) {

    int value = 0;
    int keyIndex = command.indexOf(queryFor);
    if (keyIndex == -1) {
        return; // Key not found
    }
    int motorNumIndex = keyIndex + queryFor.length() + 1;

    // int motorNumIndex = command.indexOf(ITEM_1) + 6; //QUERY MOTOR 1 SPEED
    int queryTypeIndex = command.indexOf(queryType);

    if (motorNumIndex == -1 || queryTypeIndex == -1) {
        return; // Exit if motor number is missing
    }

    motorNum = command.substring(motorNumIndex, queryTypeIndex - 1).toInt();
    if (motorNum < 1 || motorNum > 6) {
        return; // Exit if the motor number is invalid
    }
    if(motorIndex)
    {
      *motorIndex = motorNum;
    }

    COMMAND_ADD_PARAM(PARAM1)
    COMMAND_ADD_PARAM(PARAM2)
    COMMAND_ADD_PARAM(PARAM3)

    if(val){
      *val = value;
    }
}

void Manarharake::queryMotor(const String& command) {
    int motorNum;
    String queryType = COMMAND_TYPE_2;

    parseCommand(command, motorNum, queryType, ITEM_1, NULL);

    if (!queryType.isEmpty()) {
        if (queryType.equals(PARAM1)) {
            Serial.println(motor[motorNum - 1].getSpeed());
        } else if (queryType.equals(PARAM2)) {
            Serial.println(motor[motorNum - 1].getPosition());
        }
    }
}

void Manarharake::querySensor(const String& command) 
{
    int sensorIndex;
    String queryType = COMMAND_TYPE_2;

    parseCommand(command, sensorIndex, queryType, ITEM_2, NULL);

    if (!queryType.isEmpty()) {
      if (queryType.equals(PARAM3)) {
          Serial.println(motor[sensorIndex - 1].getSensorSensitivity());
      }
    }
}

void Manarharake::setMotor(const String& command) 
{
    int val;
    int motorNum;
    int index;
    String queryType = COMMAND_TYPE_1;

    parseCommand(command, index, queryType, ITEM_1, &val, &motorNum);

    if (!queryType.isEmpty()) {
      if (queryType.equals(PARAM1)) {
        motor[motorNum - 1].setSpeed(val);
      } else if (queryType.equals(PARAM2)) {
        motor[motorNum - 1].setPosition(val);
      }
    }
}

void Manarharake::setSensor(const String& command) 
{
    int val;
    int sensorIndex;
    int index;
    String queryType = COMMAND_TYPE_1;

    parseCommand(command, index, queryType, ITEM_2, &val, &sensorIndex);

    if (!queryType.isEmpty()) {
      if (queryType.equals(PARAM3)) {
        motor[sensorIndex - 1].setSensorSensitivity(val);
      }
    }
}

void Manarharake::setResistor(const String& command) 
{
    // Find the index of the comma
    int commaIndex = command.indexOf(',');

    // Check if the comma exists and there's at least one character after it
    if (commaIndex != -1 && commaIndex < command.length() - 1) {
        // Extract resistor number and value
        String resistorNumStr = command.substring(commaIndex-1, commaIndex);
        String valueStr = command.substring(commaIndex + 1);

        // Convert strings to integers
        int resistorNum = resistorNumStr.toInt();
        int value = valueStr.toInt();

        // Check if conversion is successful and resistor number is valid
        if (resistorNum >= 1 && resistorNum <= 2) {
            // Set the resistor value (replace this with actual implementation)
            // Example:
            // setResistorValue(resistorNum, value);
            Serial.print("Resistor ");
            Serial.print(resistorNum);
            Serial.print(" set to value ");
            Serial.println(value);
        } else {
            // Invalid resistor number
            Serial.print("Invalid resistor number.");
            Serial.println(resistorNumStr);
        }
    } else {
        // Incorrect command format
        Serial.println("Incorrect command format.");
    }
}