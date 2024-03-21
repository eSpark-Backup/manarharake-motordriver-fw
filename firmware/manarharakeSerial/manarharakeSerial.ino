#define ARDUINO_BOARD 1
#define TEENSY_BOARD  2

#define BOARD ARDUINO_BOARD
// #define BOARD TEENSY_BOARD

#if (BOARD == ARDUINO_BOARD)
#include <Arduino_FreeRTOS.h>
#endif

#if (BOARD == TEENSY_BOARD)
#include "arduino_freertos.h"
#endif

#include <Arduino.h>
#include <EEPROM.h>
// Function prototypes for task functions
void demoTask(void *pvParameters);

// Constants
#define LED_PIN 12
#define BLINK_DELAY_MS 250  // Default blink delay

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

const uint8_t MOTOR_PINS[] = {3, 5, 6, 9, 10, 11}; // Example pins for 6 motors
uint8_t motorSpeeds[6] = {0}; // Array to store motor speeds
uint16_t motorPositions[6] = {0}; // Array to store motor positions
uint8_t sensorSensitivities[6] = {0}; // Array to store sensor sensitivities

void setup() {
  Serial.begin(115200);
  Serial.println("Controller Started");

  // Initialize LED pin as output
  // pinMode(LED_PIN, OUTPUT);
  #if (BOARD == ARDUINO_BOARD)
  pinMode(LED_PIN, OUTPUT);
  #endif 
  #if (BOARD == TEENSY_BOARD)
  pinMode(LED_PIN, arduino::OUTPUT); // Set motor pins as output
  #endif
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
        uint8_t motorNumIndex = command.indexOf("MOTOR") + 6;  // Find the index of "MOTOR" part in the command
        uint8_t positionIndex = command.indexOf("POSITION");   // Find the index of "POSITION" part in the command
        uint8_t speedIndex = command.indexOf("SPEED");         // Find the index of "SPEED" part in the command

        if (motorNumIndex == -1 || (positionIndex == -1 && speedIndex == -1)) {
          //Serial.println("Motor number, position, or speed is missing.");
          return;  // Exit the function if any required attribute is missing
        }

        uint8_t motorNum = command.substring(motorNumIndex, min(positionIndex, speedIndex)).toInt();  // Extract motor number from command
        if (motorNum < 1 || motorNum > 6) {
          //Serial.println("Invalid motor number.");
          return;  // Exit the function if the motor number is invalid
        }

        if (positionIndex != -1) {
          uint16_t position = command.substring(positionIndex + 9).toInt();  // Extract position from command
          setMotorPosition(motorNum, position);
        }

        if (speedIndex != -1) {
          uint8_t speed = command.substring(speedIndex + 6).toInt();  // Extract speed from command
          setMotorSpeed(motorNum, speed);
        }
      } else if (command.startsWith("SET SENSOR")) {
        uint8_t sensorNumIndex = command.indexOf("SENSOR") + 7;     // Find the index of "SENSOR" part in the command
        uint8_t sensitivityIndex = command.indexOf("SENSITIVITY");  // Find the index of "SENSITIVITY" part in the command

        if (sensorNumIndex == -1 || sensitivityIndex == -1) {
          //Serial.println("Sensor number or sensitivity is missing.");
          return;  // Exit the function if either sensor number or sensitivity is missing
        }

        uint8_t sensorNum = command.substring(sensorNumIndex, sensitivityIndex).toInt();  // Extract sensor number from command
        if (sensorNum < 1 || sensorNum > 6) {
          //Serial.println("Invalid sensor number.");
          return;  // Exit the function if the sensor number is invalid
        }

        uint8_t sensitivity = command.substring(sensitivityIndex + 12).toInt();  // Extract sensitivity from command
        setSensorSensitivity(sensorNum, sensitivity);                        // Set sensitivity for the specified sensor
      } else if (command.startsWith("QUERY MOTOR")) {
        uint8_t motorNumIndex = command.indexOf("MOTOR") + 6;  // Find the index of "MOTOR" part in the command
        uint8_t queryTypeIndex = command.indexOf("QUERY");     // Find the index of "QUERY" part in the command

        if (motorNumIndex == -1 || queryTypeIndex == -1) {
          //Serial.println("Motor number is missing.");
          return;  // Exit the function if motor number is missing
        }

        uint8_t motorNum = command.substring(motorNumIndex, queryTypeIndex - 1).toInt();  // Extract motor number from command
        if (motorNum < 1 || motorNum > 6) {
          //Serial.println("Invalid motor number.");
          return;  // Exit the function if the motor number is invalid
        }

        if (command.indexOf("SPEED") != -1) {
          queryMotorSpeed(motorNum);  // Query speed for the specified motor
        } else if (command.indexOf("POSITION") != -1) {
          queryMotorPosition(motorNum);  // Query position for the specified motor
        } else {
          //Serial.println("Invalid query type.");
          return;  // Exit the function if the query type is invalid
        }
      } else if (command.startsWith("QUERY SENSOR")) {
        uint8_t sensorNumIndex = command.indexOf("SENSOR") + 6;  // Find the index of "SENSOR" part in the command

        if (sensorNumIndex == -1) {
          //Serial.println("Sensor number is missing.");
          return;  // Exit the function if sensor number is missing
        }

        uint8_t sensorNum = command.substring(sensorNumIndex).toInt();  // Extract sensor number from command
        if (sensorNum < 1 || sensorNum > 6) {
          //Serial.println("Invalid sensor number.");
          return;  // Exit the function if the sensor number is invalid
        }

        querySensorSensitivity(sensorNum);  // Query sensitivity for the specified sensor
      } else if (command.charAt(0) >= 'A' && command.charAt(0) <= 'G') {
        executeArray(command);  // Call executeArray function for array commands
      } else if (command.startsWith("START DEMO")) {
        // Create the blinking task if it doesn't exist
        // if (demoTaskHandle == NULL) {
          // //Serial.println("Blink Task Starting!");
          vTaskSuspend(printTaskHandle);
          vTaskResume(demoTaskHandle);
          // xTaskCreate(demoTask, "BlinkLED", 100, NULL, 1, &demoTaskHandle);
        // }
      } else if (command.startsWith("STOP DEMO")) {
        // Delete the blinking task if it exists
        if (demoTaskHandle != NULL) {
          // vTaskDelete(demoTaskHandle);
          vTaskResume(printTaskHandle);
          vTaskSuspend(demoTaskHandle);
          // demoTaskHandle = NULL;
          // digitalWrite(LED_PIN, LOW);  // Ensure LED is turned off
        }
      } else if (command.startsWith("SAVE CONFIG")){
        EEPROM.write(10, motorSpeeds[0]);
        EEPROM.write(11, motorSpeeds[1]);
        EEPROM.write(12, motorSpeeds[2]);
        EEPROM.write(13, motorSpeeds[3]);
        EEPROM.write(14, motorSpeeds[4]);
        EEPROM.write(15, motorSpeeds[5]);

        EEPROM.write(20, motorPositions[0]);
        EEPROM.write(21, motorPositions[1]);
        EEPROM.write(22, motorPositions[2]);
        EEPROM.write(23, motorPositions[3]);
        EEPROM.write(24, motorPositions[4]);
        EEPROM.write(25, motorPositions[5]);

        EEPROM.write(30, motorPositions[0]>>8);
        EEPROM.write(31, motorPositions[1]>>8);
        EEPROM.write(32, motorPositions[2]>>8);
        EEPROM.write(33, motorPositions[3]>>8);
        EEPROM.write(34, motorPositions[4]>>8);
        EEPROM.write(35, motorPositions[5]>>8);

      } else if (command.startsWith("LOAD CONFIG")){
        setMotorSpeed(1, EEPROM.read(10));
        setMotorSpeed(2, EEPROM.read(11));
        setMotorSpeed(3, EEPROM.read(12));
        setMotorSpeed(4, EEPROM.read(13));
        setMotorSpeed(5, EEPROM.read(14));
        setMotorSpeed(6, EEPROM.read(15));      

        setMotorPosition(1, EEPROM.read(20) | ((uint16_t)(EEPROM.read(30)) << 8));
        setMotorPosition(2, EEPROM.read(21) | ((uint16_t)(EEPROM.read(31)) << 8));
        setMotorPosition(3, EEPROM.read(22) | ((uint16_t)(EEPROM.read(32)) << 8));
        setMotorPosition(4, EEPROM.read(23) | ((uint16_t)(EEPROM.read(33)) << 8));
        setMotorPosition(5, EEPROM.read(24) | ((uint16_t)(EEPROM.read(34)) << 8));
        setMotorPosition(6, EEPROM.read(25) | ((uint16_t)(EEPROM.read(35)) << 8)); 
      } else {
        //Serial.println("Invalid command.");
        // return;  // Exit the function if the command is invalid
        // printConfiguration();
      }
    }
    vTaskDelay(1);
  }

  // //Serial.println("task ended");
}

// Task to blink LED
void demoTask(void *pvParameters) {
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

  // for (;;) {
  //   // Serial.println("Blinking");
  //   digitalWrite(LED_PIN, HIGH);  // Turn LED on
  //   vTaskDelay(BLINK_DELAY_MS / portTICK_PERIOD_MS);
  //   digitalWrite(LED_PIN, LOW);  // Turn LED off
  //   vTaskDelay(BLINK_DELAY_MS / portTICK_PERIOD_MS);
  // }
}

void printTask(void *pvParameters) {
  (void) pvParameters;
  for(;;)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    printConfiguration();
    // Serial.println("Config print disabled");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setMotorPosition(uint8_t motorNum, uint16_t position) {
  // Implement your logic to move the motor to the specified position here
  // For simplicity, we'll just set the position in the array
  motorPositions[motorNum - 1] = position;
}

void setMotorSpeed(uint8_t motorNum, uint8_t speed) {
  // Adjust speed within the range of 100 to 255
  // speed = constrain(speed, 100, 255);
  // Set motor speed using PWM
  analogWrite(MOTOR_PINS[motorNum - 1], speed);
  // Update speed in the array
  motorSpeeds[motorNum - 1] = speed;
}

void setSensorSensitivity(uint8_t sensorNum, uint8_t sensitivity) {
  // Adjust sensitivity within the range of 1 to 10
  // sensitivity = constrain(sensitivity, 1, 10);
  // Set sensitivity for the specified sensor
  sensorSensitivities[sensorNum - 1] = sensitivity;
}

void queryMotorPosition(uint8_t motorNum) {
  // Query position for the specified motor
  Serial.print("Position of Motor ");
  Serial.print(motorNum);
  Serial.print(": ");
  Serial.println(motorPositions[motorNum - 1]);
}

void queryMotorSpeed(uint8_t motorNum) {
  // Query speed for the specified motor
  Serial.print("Speed of Motor ");
  Serial.print(motorNum);
  Serial.print(": ");
  Serial.println(motorSpeeds[motorNum - 1]);
}

void querySensorSensitivity(uint8_t sensorNum) {
  // Print current sensitivity setting of the specified sensor
  Serial.print("Current sensitivity setting of Sensor ");
  Serial.print(sensorNum);
  Serial.print(": ");
  Serial.println(sensorSensitivities[sensorNum - 1]);
}

void printConfiguration() {
  // Print total configuration for all 6 motors and sensors
  Serial.println("Total Configuration:");
  // char buffer[50];

  for (uint8_t i = 0; i < 6; i++) {
    // Serial.print("Motor ");
    Serial.print(i + 1);
    Serial.print(": Speed=");
    Serial.print(motorSpeeds[i]);
    Serial.print(", Pos=");
    Serial.print(motorPositions[i]);
    // Serial.print(", Sensor ");
    // Serial.print(i + 1);
    Serial.print(": Sens=");
    Serial.println(sensorSensitivities[i]);
    // sprintf (buffer, "Motor %d Speed %d Position %d Sensor Sensitivity",i + 1, motorSpeeds[i], motorPositions[i]);
    // Serial.print(buffer);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void executeArray(String command) {
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
    
    setMotorPosition(motorNumber, position); // Set position for motor i
    setMotorSpeed(motorNumber, speed); // Set speed for motor i
  }
}
