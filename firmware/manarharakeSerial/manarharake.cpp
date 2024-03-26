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

/* Prototypes */
static void printTask(void *pvParameters);
static void demoTask(void *pvParameters);
static void mainTask(void *pvParameters);

static void printConfiguration();
static void setMotor(const String& command);
static void setSensor(const String& command);
static void queryMotor(const String& command);
static void querySensor(const String& command);
static void setResistor(const String& command);
static void loadConfiguration(const String& command);
static void saveConfiguration(const String& command);
static void startDemo(const String& command);
static void stopDemo(const String& command);
static void executeArray(String command);

// Global variables
TaskHandle_t demoTaskHandle = NULL;  // Handle for the blinking task
TaskHandle_t mainTaskHandle = NULL;   // Handle for the blinking task
TaskHandle_t printTaskHandle = NULL;   // Handle for the blinking task

Motor motors[6] = {
    Motor(100, 0, 5, M1_PIN), // Motor 1
    Motor(100, 0, 5, M2_PIN), // Motor 2
    Motor(100, 0, 5, M3_PIN), // Motor 3
    Motor(100, 0, 5, M4_PIN), // Motor 4
    Motor(100, 0, 5, M5_PIN), // Motor 5
    Motor(100, 0, 5, M6_PIN)  // Motor 6
};


// // Array to store motor positions and speeds for each array letter
// const MotorsConfig motorConfiguration[7] = {
//     {{{700, 255}, {500, 255}, {700, 255}, {0, 255}, {0, 255}, {0, 255}}},   // A
//     {{{700, 255}, {1000, 255}, {700, 255}, {700, 255}, {0, 255}, {0, 255}}}, // B
//     {{{900, 255}, {900, 255}, {900, 255}, {900, 255}, {900, 255}, {900, 255}}}, // C
//     {{{0, 255}, {500, 255}, {900, 255}, {900, 255}, {900, 255}, {900, 255}}}, // D
//     {{{800, 255}, {500, 255}, {500, 255}, {0, 255}, {0, 255}, {0, 255}}},     // E
//     {{{700, 255}, {700, 255}, {850, 255}, {850, 255}, {850, 255}, {850, 255}}}, // F
//     {{{0, 255}, {0, 255}, {0, 255}, {0, 255}, {0, 255}, {0, 255}}}             // G
// };

// Define MotorSettings structure
struct MotorSettings {
    int position;
    int speed;
};

// Define MotorsConfig structure
struct MotorsConfig {
    MotorSettings array_motors[6];
};

// Array to store motor positions and speeds for each array letter
const MotorsConfig motorConfiguration[7] = {
    {{{700,   0   },    {500,   50  }, {700,  100 }, {0,    150 }, {0,    200 }, {0,    255 }}},    // A
    {{{700,   150 },    {1000,  0   }, {700,  255 }, {700,  150 }, {0,    255 }, {0,    255 }}},    // B
    {{{900,   255 },    {900,   255 }, {900,  0   }, {900,  255 }, {900,  255 }, {900,  255 }}},    // C
    {{{0,     255 },    {500,   150 }, {900,  180 }, {900,  0   }, {900,  255 }, {900,  255 }}},    // D
    {{{800,   180 },    {500,   255 }, {500,  255 }, {0,    150 }, {0,    0   }, {0,    255 }}},    // E
    {{{700,   255 },    {700,   255 }, {850,  255 }, {850,  255 }, {850,  180 }, {850,  0   }}},    // F
    {{{0,     0   },    {0,     50  }, {0,    50  }, {0,    0   }, {0,    50  }, {0,    0   }}}     // G
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
    vTaskStartScheduler();
    loadConfiguration("NULL");
    Serial.println("Controller Started");
}

struct SerialCommand{
  char* commandString;
  void(*commandCallback)(const String& command);
};

SerialCommand commands[] = {
  {"SET MOTOR", setMotor},
  {"SET SENSOR", setSensor},
  {"QUERY MOTOR", queryMotor},
  {"QUERY SENSOR", querySensor},
  {"SET R", setResistor},
  {"START DEMO", startDemo},
  {"STOP DEMO", stopDemo},
  {"SAVE CONFIG", saveConfiguration},
  {"LOAD CONFIG", loadConfiguration}
};


static void mainTask(void *pvParameters) {
  
  for(;;) {
    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n');  // Read the command until newline character
      for(int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++)
      {
        if(command.startsWith(commands[i].commandString))
        {
          commands[i].commandCallback(command);
          break;
        } else if (command.charAt(0) >= CONFIG_A && command.charAt(0) <= CONFIG_G) {
          executeArray(command);  // Call executeArray function for array commands
          break;
        }
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
      executeArray(String(demoArray)); // Execute arrays A-F sequentially
      vTaskDelay(1000/portTICK_PERIOD_MS); // Delay 1 second
      
      demoArray = CONFIG_G;
      executeArray(String(demoArray)); // Execute array G after arrays A-F
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
    printConfiguration();
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

static void printConfiguration() 
{
  // Print total configuration for all 6 motors and sensors
  Serial.println("Total Configuration:");

  for (int i = 0; i < MOTOR_COUNT; i++) {
    Serial.print(i + 1);
    Serial.print(": Speed=");
    Serial.print(motors[i].getSpeed());
    Serial.print(", Pos=");
    Serial.print(motors[i].getPosition());
    Serial.print(", Sensor ");
    Serial.print(i + 1);
    Serial.print(": Sens=");
    Serial.println(motors[i].getSensorSensitivity());
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

static void executeArray(String command) 
{
  char arrayLetter = command.charAt(0); // Extract array letter from command
  if (arrayLetter < CONFIG_A || arrayLetter > CONFIG_G) {
    Serial.println("Invalid array letter.");
    return; // Exit the function if the array letter is invalid
  }
  
  int arrayIndex = arrayLetter - CONFIG_A; // Calculate array index
  
  // Execute movements for each motor
  for (int i = 0; i < sizeof(motors)/sizeof(motors[0]); i++) {
    // int position = motorConfiguration[arrayIndex][i * 2]; // Get position value for motor i
    // int speed = motorConfiguration[arrayIndex][i * 2 + 1]; // Get speed value for motor i

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
    
    motors[motorNumber - 1].setSpeed(speed); // Set speed for motor i
    motors[motorNumber - 1].setPosition(position); // Set speed for motor i
  }
}

static void loadConfiguration(const String& command)
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

      motors[i].setSpeed(speed);
      motors[i].setPosition(positionLow | (positionHigh << 8));
      motors[i].setSensorSensitivity(sensorSensitivity);
  }
}

static void saveConfiguration(const String& command)
{
  for(int i = 0; i < MOTOR_COUNT; i++) {
      int speedAddress = SPEED_OFFSET + i;
      int positionLowAddress = POSITION_LOW_OFFSET + i;
      int positionHighAddress = POSITION_HIGH_OFFSET + i;
      int sensorSensitivityAddress = SENSOR_SENSITIVITY_OFFSET + i;

      int speed = motors[i].getSpeed();
      int position = motors[i].getPosition();
      int sensorSensitivity = motors[i].getSensorSensitivity();

      EEPROM.write(speedAddress, speed);
      EEPROM.write(positionLowAddress, position & 0xFF);  // Low byte
      EEPROM.write(positionHighAddress, (position >> 8) & 0xFF);  // High byte
      EEPROM.write(sensorSensitivityAddress, sensorSensitivity);
  }
  EEPROM.commit();
}

static void stopDemo(const String& command)
{
  // Delete the blinking task if it exists
  if (demoTaskHandle != NULL) {
    vTaskResume(printTaskHandle);
    vTaskSuspend(demoTaskHandle);
  }
}

static void startDemo(const String& command)
{
  vTaskSuspend(printTaskHandle);
  vTaskResume(demoTaskHandle);
}

static void setSensor(const String& command) 
{
    int sensorNumIndex = command.indexOf("SENSOR") + 7;
    int sensitivityIndex = command.indexOf("SENSITIVITY");
    if (sensorNumIndex == -1 || sensitivityIndex == -1) {
        Serial.println("sensor number or sensitivity is missing");
        return; // Exit if either sensor number or sensitivity is missing
    }

    int sensorNum = command.substring(sensorNumIndex, sensitivityIndex).toInt();
    if (sensorNum < 1 || sensorNum > 6) {
        Serial.println("sensor number is invalid");
        return; // Exit if the sensor number is invalid
    }

    int sensitivity = command.substring(sensitivityIndex + 12).toInt();
    motors[sensorNum - 1].setSensorSensitivity(sensitivity);
}

static void queryMotor(const String& command) 
{
    int motorNumIndex = command.indexOf("MOTOR") + 6;
    int queryTypeIndex = command.indexOf("QUERY");

    if (motorNumIndex == -1 || queryTypeIndex == -1) {
        return; // Exit if motor number is missing
    }

    int motorNum = command.substring(motorNumIndex, queryTypeIndex - 1).toInt();
    if (motorNum < 1 || motorNum > 6) {
        return; // Exit if the motor number is invalid
    }

    if (command.indexOf("SPEED") != -1) {
        motors[motorNum - 1].getSpeed();
    } else if (command.indexOf("POSITION") != -1) {
        motors[motorNum - 1].getPosition();
    }
}

static void querySensor(const String& command) 
{
    int sensorNumIndex = command.indexOf("SENSOR") + 6;

    if (sensorNumIndex == -1) {
        return; // Exit if sensor number is missing
    }

    int sensorNum = command.substring(sensorNumIndex).toInt();
    if (sensorNum < 1 || sensorNum > 6) {
        return; // Exit if the sensor number is invalid
    }

    motors[sensorNum - 1].getSensorSensitivity();
}

static void setMotor(const String& command) 
{
    int motorNumIndex = command.indexOf("MOTOR") + 6;
    int positionIndex = command.indexOf("POSITION");
    int speedIndex = command.indexOf("SPEED");

    if (motorNumIndex == -1 || (positionIndex == -1 && speedIndex == -1)) {
        Serial.println("attribute is missing");
        return; // Exit if any required attribute is missing
    }

    int motorNum = command.substring(motorNumIndex, min(positionIndex, speedIndex)).toInt();
    if (motorNum < 1 || motorNum > 6) {
        Serial.println("motor number is invalid");
        return; // Exit if the motor number is invalid
    }

    if (positionIndex != -1) {
        int position = command.substring(positionIndex + 9).toInt();
        motors[motorNum - 1].setPosition(position);
    }

    if (speedIndex != -1) {
        int speed = command.substring(speedIndex + 6).toInt();
        motors[motorNum - 1].setSpeed(speed);
    }
}

static void setResistor(const String& command) 
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