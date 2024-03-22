#include "board.h"
#if (BOARD == ARDUINO_BOARD)
#include <Arduino_FreeRTOS.h>
#endif

#if (BOARD == TEENSY_BOARD)
#include "arduino_freertos.h"
#endif

#include <Arduino.h>
#include <EEPROM.h>
#include "motor_control.hpp"

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
// const int arrayValues[7][12] = {
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
const int arrayValues[7][12] = {
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
      } else if (command.startsWith("SET R")) {
        setResistor(command);
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

// Task to execute predefined config array
void demoTask(void *pvParameters) 
{
  (void)pvParameters;

  char demoArray;
  for(;;) {
    for (int i = 0; i < 6; i++) {
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

  for (int i = 0; i < 6; i++) {
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
  
  int arrayIndex = arrayLetter - 'A'; // Calculate array index
  
  // Execute movements for each motor
  for (int i = 0; i < 6; i++) {
    int position = arrayValues[arrayIndex][i * 2]; // Get position value for motor i
    int speed = arrayValues[arrayIndex][i * 2 + 1]; // Get speed value for motor i
    
    // Adjust motor number according to the changed numbering (m6, m5, m1, m2, m3, m4)
    int motorNumber;
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

  motors[1 - 1].setPosition(EEPROM.read(20) | ((int)(EEPROM.read(30)) << 8));
  motors[2 - 1].setPosition(EEPROM.read(21) | ((int)(EEPROM.read(31)) << 8));
  motors[3 - 1].setPosition(EEPROM.read(22) | ((int)(EEPROM.read(32)) << 8));
  motors[4 - 1].setPosition(EEPROM.read(23) | ((int)(EEPROM.read(33)) << 8));
  motors[5 - 1].setPosition(EEPROM.read(24) | ((int)(EEPROM.read(34)) << 8));
  motors[6 - 1].setPosition(EEPROM.read(25) | ((int)(EEPROM.read(35)) << 8)); 

  motors[1 - 1].setSensorSensitivity(EEPROM.read(40));
  motors[2 - 1].setSensorSensitivity(EEPROM.read(41));
  motors[3 - 1].setSensorSensitivity(EEPROM.read(42));
  motors[4 - 1].setSensorSensitivity(EEPROM.read(43));
  motors[5 - 1].setSensorSensitivity(EEPROM.read(44));
  motors[6 - 1].setSensorSensitivity(EEPROM.read(45));

}

void saveConfiguration()
{
  EEPROM.update(10, motors[0].getSpeed());
  EEPROM.update(11, motors[1].getSpeed());
  EEPROM.update(12, motors[2].getSpeed());
  EEPROM.update(13, motors[3].getSpeed());
  EEPROM.update(14, motors[4].getSpeed());
  EEPROM.update(15, motors[5].getSpeed());

  EEPROM.update(20, motors[0].getPosition());
  EEPROM.update(21, motors[1].getPosition());
  EEPROM.update(22, motors[2].getPosition());
  EEPROM.update(23, motors[3].getPosition());
  EEPROM.update(24, motors[4].getPosition());
  EEPROM.update(25, motors[5].getPosition());

  EEPROM.update(30, motors[0].getPosition()>>8);
  EEPROM.update(31, motors[1].getPosition()>>8);
  EEPROM.update(32, motors[2].getPosition()>>8);
  EEPROM.update(33, motors[3].getPosition()>>8);
  EEPROM.update(34, motors[4].getPosition()>>8);
  EEPROM.update(35, motors[5].getPosition()>>8);

  EEPROM.update(40, motors[0].getSensorSensitivity());
  EEPROM.update(41, motors[1].getSensorSensitivity());
  EEPROM.update(42, motors[2].getSensorSensitivity());
  EEPROM.update(43, motors[3].getSensorSensitivity());
  EEPROM.update(44, motors[4].getSensorSensitivity());
  EEPROM.update(45, motors[5].getSensorSensitivity());

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

void queryMotor(const String& command) 
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

void querySensor(const String& command) 
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

void setMotor(const String& command) 
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

void setResistor(const String& command) 
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

