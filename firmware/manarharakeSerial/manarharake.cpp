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
static void checkFsrTask(void *pvParameters);
static void maintainPosTask(void *pvParameters);

// Global variables
TaskHandle_t demoTaskHandle = NULL;  // Handle for the Array demo task
TaskHandle_t mainTaskHandle = NULL;   // Handle for the main task
TaskHandle_t printTaskHandle = NULL;   // Handle for the print config task
TaskHandle_t fsrTaskHandle = NULL;   // Handle for the check fsr task
TaskHandle_t maintainPosTaskHandle = NULL;   // Handle for the maintain motor pos task

// Definition FSR object to be assigned to pointer
ForceSensitiveResistor fsr[FSR_COUNT] = {
  ForceSensitiveResistor(FSR1_PIN),
  ForceSensitiveResistor(FSR2_PIN),
  ForceSensitiveResistor(FSR3_PIN),
  ForceSensitiveResistor(FSR4_PIN),
  ForceSensitiveResistor(FSR5_PIN),
};
// Definition of static member variable
Motor Manarharake::motor[MOTOR_COUNT] = {
    Motor({DEFAULT_SPEED, DEFAULT_POSITION, DEFAULT_SENSITIVITY, M1_PIN, M1_PIN2}, &fsr[0]), // Motor 1
    Motor({DEFAULT_SPEED, DEFAULT_POSITION, DEFAULT_SENSITIVITY, M2_PIN, M2_PIN2}, &fsr[1]), // Motor 2
    Motor({DEFAULT_SPEED, DEFAULT_POSITION, DEFAULT_SENSITIVITY, M3_PIN, M3_PIN2}, &fsr[2]), // Motor 3
    Motor({DEFAULT_SPEED, DEFAULT_POSITION, DEFAULT_SENSITIVITY, M4_PIN, M4_PIN2}, &fsr[3]), // Motor 4
    Motor({DEFAULT_SPEED, DEFAULT_POSITION, DEFAULT_SENSITIVITY, M5_PIN, M5_PIN2}, &fsr[4]), // Motor 5
    Motor({DEFAULT_SPEED, DEFAULT_POSITION, DEFAULT_SENSITIVITY, M6_PIN, M6_PIN2}, &fsr[4])  // Motor 6
};

// Command List
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
    Serial.begin(BAUDRATE);
    #if(BOARD == TEENSY_BOARD)
    EEPROM.begin();
    #else
    EEPROM.begin(EEPROM_SIZE_BYTES);
    #endif

    // Create Tasks
    xTaskCreate(printTask, "printTask", 2*2048, NULL, 0, &printTaskHandle);
    xTaskCreate(mainTask, "mainTask", 2*2048, NULL, 1, &mainTaskHandle);
    xTaskCreate(demoTask, "arrayDemoTask", 2*2048, NULL, 2, &demoTaskHandle);
    vTaskSuspend(demoTaskHandle);
    xTaskCreate(checkFsrTask, "fsrTask", 2*2048, NULL, 2, &fsrTaskHandle);
    vTaskSuspend(fsrTaskHandle);
    xTaskCreate(maintainPosTask, "MotorPosition", 2*2048, NULL, 3, &maintainPosTaskHandle);
    // vTaskStartScheduler();

    loadConfiguration("NULL");
    MH_LOG_LN("Controller Started");
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
    for (int i = 0; i < MOTOR_COUNT; i++) {
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

static void checkFsrTask(void *pvParameters)
{
  (void) pvParameters;
  vTaskDelay(1000/portTICK_PERIOD_MS);
  for(;;)
  {
      Manarharake::checkFsrThreshold();
      vTaskDelay(10);
  }
}

void Manarharake::checkFsrThreshold()
{
    for(int i = 0; i < MOTOR_COUNT; i++)
    {
      motor[i].checkFsr();
    }
}

uint16_t global_encoder[MOTOR_COUNT] = {0}; //0 - 65535 3FF = 1023 //TODO: Proper Encode Implementation

static void maintainPosTask(void *pvParameters)
{
  (void) pvParameters;
  for(;;)
  {
      Manarharake::maintainPos();
      vTaskDelay(10);
  }
}

void Manarharake::maintainPos()
{
  for(int i = 0; i < MOTOR_COUNT; i++)
  {
    global_encoder[i] &= 0x3FF;  
    if(global_encoder[i] < motor[i].getPosition())
    {
      motor[i].setDirection(CW);
      motor[i].run();
      global_encoder[i]++;
    }
    if(global_encoder[i] > motor[i].getPosition()){
      motor[i].setDirection(CCW);
      motor[i].run();
      global_encoder[i]--;
    }
    if(global_encoder[i] == motor[i].getPosition()){
      motor[i].stop();
    }
  }
}

void Manarharake::printConfiguration() 
{
  // Print total configuration for all 6 motors and sensors
  MH_LOG_LN("Total Configuration:");

  for (int i = 0; i < MOTOR_COUNT; i++) {
    MH_LOG(i + 1);
    MH_LOG(": Speed = ");
    MH_LOG(motor[i].getSpeed());
    MH_LOG("\t, Pos =");
    MH_LOG(motor[i].getPosition());
    MH_LOG("\t, Sensor ");
    MH_LOG(i + 1);
    MH_LOG(": Current Sens Lvl = ");
    MH_LOG(motor[i].readCurrentFsrVal()); //Read current Analog level
    MH_LOG("\t: Sens =");
    MH_LOG(motor[i].getSensorSensitivity());
    MH_LOG("\t Encoder Position ");
    MH_LOG_LN(global_encoder[i]);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void Manarharake::executeArray(char command) 
{
  char arrayLetter = command;
  if (arrayLetter < CONFIG_A || arrayLetter > CONFIG_G) {
    MH_LOG_LN("Invalid array letter.");
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
  #if(BOARD !=  TEENSY_BOARD)
  EEPROM.commit();
  #endif
}

void Manarharake::stopDemo(const String& command)
{
  if (demoTaskHandle != NULL) {
    // vTaskResume(printTaskHandle);
    vTaskSuspend(demoTaskHandle);
    vTaskSuspend(fsrTaskHandle);
    loadConfiguration("NULL");
  }
}

void Manarharake::startDemo(const String& command)
{
  // vTaskSuspend(printTaskHandle);
  vTaskResume(demoTaskHandle);
  vTaskResume(fsrTaskHandle);
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

    queryType = "";
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

    if (queryType != "") {
        if (queryType.equals(PARAM1)) {
            MH_LOG_LN(motor[motorNum - 1].getSpeed());
        } else if (queryType.equals(PARAM2)) {
            MH_LOG_LN(motor[motorNum - 1].getPosition());
        }
    }
}

void Manarharake::querySensor(const String& command) 
{
    int sensorIndex;
    String queryType = COMMAND_TYPE_2;

    parseCommand(command, sensorIndex, queryType, ITEM_2, NULL);

    if (queryType != "") {
      if (queryType.equals(PARAM3)) {
          MH_LOG_LN(motor[sensorIndex - 1].getSensorSensitivity());
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

    if (queryType != "") {
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

    if (queryType != "") {
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
            MH_LOG("Resistor ");
            MH_LOG(resistorNum);
            MH_LOG(" set to value ");
            MH_LOG_LN(value);
        } else {
            // Invalid resistor number
            MH_LOG("Invalid resistor number.");
            MH_LOG_LN(resistorNumStr);
        }
    } else {
        // Incorrect command format
        MH_LOG_LN("Incorrect command format.");
    }
}
