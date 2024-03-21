#if (BOARD == ARDUINO_BOARD)
#include <Arduino_FreeRTOS.h>
#endif

#if (BOARD == TEENSY_BOARD)
#include "arduino_freertos.h"
#endif

#include <Arduino.h>
#include <EEPROM.h>
#include "motor_control.hpp"
#include "board.h"

Motor motors[6] = {
    Motor(100, 0, 5, M1_PIN), // Motor 1
    Motor(100, 0, 5, M2_PIN), // Motor 2
    Motor(100, 0, 5, M3_PIN), // Motor 3
    Motor(100, 0, 5, M4_PIN), // Motor 4
    Motor(100, 0, 5, M5_PIN), // Motor 5
    Motor(100, 0, 5, M6_PIN)  // Motor 6
};

// Global variables
TaskHandle_t demoTaskHandle = NULL;  // Handle for the blinking task
TaskHandle_t mainTaskHandle = NULL;   // Handle for the blinking task
TaskHandle_t printTaskHandle = NULL;   // Handle for the blinking task

// // Array to store motor positions and speeds for each array letter
// const uint16_t arrayValues[7][12] = {
//   /*    M6          M5             M1            M2           M3            M4          */
//   /*pos   speed pos     speed pos     speed pos     speed pos     speed pos     speed   */
//   {700,   255,  500,    255,  700,    255,  0,      255,  0,      255,  0,      255},   // A
//   {700,   255,  1000,   255,  700,    255,  700,    255,  0,      255,  0,      255},   // B
//   {900,   255,  900,    255,  900,    255,  900,    255,  900,    255,  900,    255},   // C
//   {0,     255,  500,    255,  900,    255,  900,    255,  900,    255,  900,    255},   // D
//   {800,   255,  500,    255,  500,    255,  0,      255,  0,      255,  0,      255},   // E
//   {700,   255,  700,    255,  850,    255,  850,    255,  850,    255,  850,    255},   // F
//   {0,     255,  0,      255,  0,      255,  0,      255,  0,      255,  0,      255}    // G
// };

// Array to store motor positions and speeds for each array letter
const uint16_t arrayValues[7][12] = {
  /*    M6          M5             M1            M2           M3            M4          */
  /*pos   speed pos     speed pos     speed pos     speed pos     speed pos     speed   */
  {700,   0,    500,    50,   700,    100,  0,      150,  0,      200,  0,      255},   // A
  {700,   150,  1000,   0,    700,    255,  700,    150,  0,      255,  0,      255},   // B
  {900,   255,  900,    255,  900,    0,    900,    255,  900,    255,  900,    255},   // C
  {0,     255,  500,    150,  900,    180,  900,    0,    900,    255,  900,    255},   // D
  {800,   180,  500,    255,  500,    255,  0,      150,  0,      0,    0,      255},   // E
  {700,   255,  700,    255,  850,    255,  850,    255,  850,    180,  850,    0},   // F
  {0,     0,    0,      50,   0,      50,   0,      0,    0,      50,   0,      0}    // G
};

void setup() {
  Serial.begin(115200);
  Serial.println("Controller Started");
  // Create Tasks
  xTaskCreate(printTask, "printTask", 100, NULL, 0, &printTaskHandle);
  xTaskCreate(mainTask, "mainTask", 128, NULL, 1, &mainTaskHandle);
  xTaskCreate(demoTask, "BlinkLED", 64, NULL, 2, &demoTaskHandle);
  vTaskSuspend(demoTaskHandle);
  vTaskStartScheduler();
}

void loop() {
}

void mainTask(void *pvParameters) {
  
  for(;;) {
    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n');  // Read the command until newline character

      if (command.startsWith("SET MOTOR")) {
        setMotor(command);
      } else if (command.startsWith("SET SENSOR")) {
        setSensor(command);                      // Set sensitivity for the specified sensor
      } else if (command.startsWith("QUERY MOTOR")) {
        queryMotor(command);
      } else if (command.startsWith("QUERY SENSOR")) {
        querySensor(command);
      } else if (command.charAt(0) >= 'A' && command.charAt(0) <= 'G') {
        executeArray(command);  // Call executeArray function for array commands
      } else if (command.startsWith("START DEMO")) {
        startDemo();
      } else if (command.startsWith("STOP DEMO")) {
        stopDemo();
      } else if (command.startsWith("SAVE CONFIG")){
        saveConfiguration();
      } else if (command.startsWith("LOAD CONFIG")){
        loadConfiguration();
      } else {
        //Serial.println("Invalid command.");
        // return;  // Exit the function if the command is invalid
      }
    }
    vTaskDelay(1);
  }
}

// Task to Print current motor config
void demoTask(void *pvParameters) 
{
  (void)pvParameters;

  char demoArray;
  for(;;) {
    for (uint8_t i = 0; i < 6; i++) {
      demoArray = char('A' + i);
      executeArray(String(demoArray)); // Execute arrays A-F sequentially
      vTaskDelay(1000/portTICK_PERIOD_MS); // Delay 1 second
      
      demoArray = 'G';
      executeArray(String(demoArray)); // Execute array G after arrays A-F
      vTaskDelay(1000/portTICK_PERIOD_MS); // Delay 1 second
    }
  }
}

void printTask(void *pvParameters) 
{
  (void) pvParameters;
  for(;;)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    printConfiguration();
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void printConfiguration() 
{
  // Print total configuration for all 6 motors and sensors
  Serial.println("Total Configuration:");

  for (uint8_t i = 0; i < 6; i++) {
    Serial.print(i + 1);
    Serial.print(": Speed=");
    Serial.print(motors[i].getSpeed());
    Serial.print(", Pos=");
    Serial.print(motors[i].getPosition());
    // Serial.print(", Sensor ");
    // Serial.print(i + 1);
    Serial.print(": Sens=");
    Serial.println(motors[i].getSensorSensitivity());
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void executeArray(String command) 
{
  char arrayLetter = command.charAt(0); // Extract array letter from command
  if (arrayLetter < 'A' || arrayLetter > 'G') {
    Serial.println("Invalid array letter.");
    return; // Exit the function if the array letter is invalid
  }
  
  uint8_t arrayIndex = arrayLetter - 'A'; // Calculate array index
  
  // Execute movements for each motor
  for (uint8_t i = 0; i < 6; i++) {
    uint16_t position = arrayValues[arrayIndex][i * 2]; // Get position value for motor i
    uint8_t speed = arrayValues[arrayIndex][i * 2 + 1]; // Get speed value for motor i
    
    // Adjust motor number according to the changed numbering (m6, m5, m1, m2, m3, m4)
    uint8_t motorNumber;
    switch(i) {
      case 0: motorNumber = 6; break;
      case 1: motorNumber = 5; break;
      case 2: motorNumber = 1; break;
      case 3: motorNumber = 2; break;
      case 4: motorNumber = 3; break;
      case 5: motorNumber = 4; break;
    }
    
    motors[motorNumber - 1].setSpeed(speed); // Set speed for motor i
    motors[motorNumber - 1].setPosition(position); // Set speed for motor i
  }
}

void loadConfiguration()
{
  motors[1 - 1].setSpeed(EEPROM.read(10));
  motors[2 - 1].setSpeed(EEPROM.read(11));
  motors[3 - 1].setSpeed(EEPROM.read(12));
  motors[4 - 1].setSpeed(EEPROM.read(13));
  motors[5 - 1].setSpeed(EEPROM.read(14));
  motors[6 - 1].setSpeed(EEPROM.read(15));      

  motors[1 - 1].setPosition(EEPROM.read(20) | ((uint16_t)(EEPROM.read(30)) << 8));
  motors[2 - 1].setPosition(EEPROM.read(21) | ((uint16_t)(EEPROM.read(31)) << 8));
  motors[3 - 1].setPosition(EEPROM.read(22) | ((uint16_t)(EEPROM.read(32)) << 8));
  motors[4 - 1].setPosition(EEPROM.read(23) | ((uint16_t)(EEPROM.read(33)) << 8));
  motors[5 - 1].setPosition(EEPROM.read(24) | ((uint16_t)(EEPROM.read(34)) << 8));
  motors[6 - 1].setPosition(EEPROM.read(25) | ((uint16_t)(EEPROM.read(35)) << 8)); 
}

void saveConfiguration()
{
  EEPROM.write(10, motors[0].getSpeed());
  EEPROM.write(11, motors[1].getSpeed());
  EEPROM.write(12, motors[2].getSpeed());
  EEPROM.write(13, motors[3].getSpeed());
  EEPROM.write(14, motors[4].getSpeed());
  EEPROM.write(15, motors[5].getSpeed());

  EEPROM.write(20, motors[0].getPosition());
  EEPROM.write(21, motors[1].getPosition());
  EEPROM.write(22, motors[2].getPosition());
  EEPROM.write(23, motors[3].getPosition());
  EEPROM.write(24, motors[4].getPosition());
  EEPROM.write(25, motors[5].getPosition());

  EEPROM.write(30, motors[0].getPosition()>>8);
  EEPROM.write(31, motors[1].getPosition()>>8);
  EEPROM.write(32, motors[2].getPosition()>>8);
  EEPROM.write(33, motors[3].getPosition()>>8);
  EEPROM.write(34, motors[4].getPosition()>>8);
  EEPROM.write(35, motors[5].getPosition()>>8);
}

void stopDemo()
{
  // Delete the blinking task if it exists
  if (demoTaskHandle != NULL) {
  vTaskResume(printTaskHandle);
  vTaskSuspend(demoTaskHandle);
  }
}

void startDemo()
{
  vTaskSuspend(printTaskHandle);
  vTaskResume(demoTaskHandle);
}

void setSensor(const String& command) 
{
    uint8_t sensorNumIndex = command.indexOf("SENSOR") + 7;
    uint8_t sensitivityIndex = command.indexOf("SENSITIVITY");

    if (sensorNumIndex == -1 || sensitivityIndex == -1) {
        return; // Exit if either sensor number or sensitivity is missing
    }

    uint8_t sensorNum = command.substring(sensorNumIndex, sensitivityIndex).toInt();
    if (sensorNum < 1 || sensorNum > 6) {
        return; // Exit if the sensor number is invalid
    }

    uint8_t sensitivity = command.substring(sensitivityIndex + 12).toInt();
    motors[sensorNum - 1].setSensorSensitivity(sensitivity);
}

void queryMotor(const String& command) 
{
    uint8_t motorNumIndex = command.indexOf("MOTOR") + 6;
    uint8_t queryTypeIndex = command.indexOf("QUERY");

    if (motorNumIndex == -1 || queryTypeIndex == -1) {
        return; // Exit if motor number is missing
    }

    uint8_t motorNum = command.substring(motorNumIndex, queryTypeIndex - 1).toInt();
    if (motorNum < 1 || motorNum > 6) {
        return; // Exit if the motor number is invalid
    }

    if (command.indexOf("SPEED") != -1) {
        motors[motorNum - 1].getSpeed();
    } else if (command.indexOf("POSITION") != -1) {
        motors[motorNum - 1].getPosition();
    }
}

void querySensor(const String& command) 
{
    uint8_t sensorNumIndex = command.indexOf("SENSOR") + 6;

    if (sensorNumIndex == -1) {
        return; // Exit if sensor number is missing
    }

    uint8_t sensorNum = command.substring(sensorNumIndex).toInt();
    if (sensorNum < 1 || sensorNum > 6) {
        return; // Exit if the sensor number is invalid
    }

    motors[sensorNum - 1].getSensorSensitivity();
}

void setMotor(const String& command) 
{
    uint8_t motorNumIndex = command.indexOf("MOTOR") + 6;
    uint8_t positionIndex = command.indexOf("POSITION");
    uint8_t speedIndex = command.indexOf("SPEED");

    if (motorNumIndex == -1 || (positionIndex == -1 && speedIndex == -1)) {
        return; // Exit if any required attribute is missing
    }

    uint8_t motorNum = command.substring(motorNumIndex, min(positionIndex, speedIndex)).toInt();
    if (motorNum < 1 || motorNum > 6) {
        return; // Exit if the motor number is invalid
    }

    if (positionIndex != -1) {
        uint16_t position = command.substring(positionIndex + 9).toInt();
        motors[motorNum - 1].setPosition(position);
    }

    if (speedIndex != -1) {
        uint8_t speed = command.substring(speedIndex + 6).toInt();
        motors[motorNum - 1].setSpeed(speed);
    }
}