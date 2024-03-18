#define DEMO_DELAY 1000

/* Function Prototypes */
void printConfiguration();
void queryMotorSpeed(int motorNum);
void queryMotorPosition(int motorNum);
void setMotorSpeed(int motorNum, int speed);
void querySensorSensitivity(int sensorNum);
void setSensorSensitivity(int sensorNum, int sensitivity);
void setMotorPosition(int motorNum, int position);
void executeArray(String command);
void startDemoMode();
void stopDemoMode();

// Array to store motor positions and speeds for each array letter
int arrayValues[7][12] = {
  /*    M6          M5             M1            M2           M3            M4          */
  /*pos   speed pos     speed pos     speed pos     speed pos     speed pos     speed   */
  {700,   255,  500,    255,  700,    255,  0,      255,  0,      255,  0,      255},   // A
  {700,   255,  1000,   255,  700,    255,  700,    255,  0,      255,  0,      255},   // B
  {900,   255,  900,    255,  900,    255,  900,    255,  900,    255,  900,    255},   // C
  {0,     255,  500,    255,  900,    255,  900,    255,  900,    255,  900,    255},   // D
  {800,   255,  500,    255,  500,    255,  0,      255,  0,      255,  0,      255},   // E
  {700,   255,  700,    255,  850,    255,  850,    255,  850,    255,  850,    255},   // F
  {0,     255,  0,      255,  0,      255,  0,      255,  0,      255,  0,      255}    // G
};


const int MOTOR_PINS[] = {9, 10, 11, 12, 13, 14}; // Example pins for 6 motors
int motorSpeeds[6] = {0}; // Array to store motor speeds
int motorPositions[6] = {0}; // Array to store motor positions
int sensorSensitivities[6] = {0}; // Array to store sensor sensitivities
bool demoRunning = false;

void setup() {
  Serial.begin(9600); // Initialize serial communication
  for (int i = 0; i < 6; i++) {
    pinMode(MOTOR_PINS[i], OUTPUT); // Set motor pins as output
  }
}

int CurrentMillis = 0;
void loop() {
  if (Serial.available() > 0) { // Check if there is serial data available
    String command = Serial.readStringUntil('\n'); // Read the command until newline character
    executeCommand(command); // Execute the command
  }
  // if(CurrentMillis - millis() >= 2000)
}

void executeCommand(String command) {
  if (command.startsWith("SET MOTOR")) {
    int motorNumIndex = command.indexOf("MOTOR") + 6; // Find the index of "MOTOR" part in the command
    int positionIndex = command.indexOf("POSITION"); // Find the index of "POSITION" part in the command
    int speedIndex = command.indexOf("SPEED"); // Find the index of "SPEED" part in the command

    if (motorNumIndex == -1 || (positionIndex == -1 && speedIndex == -1)) {
      Serial.println("Motor number, position, or speed is missing.");
      return; // Exit the function if any required attribute is missing
    }

    int motorNum = command.substring(motorNumIndex, min(positionIndex, speedIndex)).toInt(); // Extract motor number from command
    if (motorNum < 1 || motorNum > 6) {
      Serial.println("Invalid motor number.");
      return; // Exit the function if the motor number is invalid
    }

    if (positionIndex != -1) {
      int position = command.substring(positionIndex + 9).toInt(); // Extract position from command
      setMotorPosition(motorNum, position);
    }

    if (speedIndex != -1) {
      int speed = command.substring(speedIndex + 6).toInt(); // Extract speed from command
      setMotorSpeed(motorNum, speed);
    }
  }  else if (command.startsWith("SET SENSOR")) {
    int sensorNumIndex = command.indexOf("SENSOR") + 7; // Find the index of "SENSOR" part in the command
    int sensitivityIndex = command.indexOf("SENSITIVITY"); // Find the index of "SENSITIVITY" part in the command

    if (sensorNumIndex == -1 || sensitivityIndex == -1) {
      Serial.println("Sensor number or sensitivity is missing.");
      return; // Exit the function if either sensor number or sensitivity is missing
    }

    int sensorNum = command.substring(sensorNumIndex, sensitivityIndex).toInt(); // Extract sensor number from command
    if (sensorNum < 1 || sensorNum > 6) {
      Serial.println("Invalid sensor number.");
      return; // Exit the function if the sensor number is invalid
    }

    int sensitivity = command.substring(sensitivityIndex + 12).toInt(); // Extract sensitivity from command
    setSensorSensitivity(sensorNum, sensitivity); // Set sensitivity for the specified sensor
  } else if (command.startsWith("QUERY MOTOR")) {
    int motorNumIndex = command.indexOf("MOTOR") + 6; // Find the index of "MOTOR" part in the command
    int queryTypeIndex = command.indexOf("QUERY"); // Find the index of "QUERY" part in the command

    if (motorNumIndex == -1 || queryTypeIndex == -1) {
      Serial.println("Motor number is missing.");
      return; // Exit the function if motor number is missing
    }

    int motorNum = command.substring(motorNumIndex, queryTypeIndex - 1).toInt(); // Extract motor number from command
    if (motorNum < 1 || motorNum > 6) {
      Serial.println("Invalid motor number.");
      return; // Exit the function if the motor number is invalid
    }

    if (command.indexOf("SPEED") != -1) {
      queryMotorSpeed(motorNum); // Query speed for the specified motor
    } else if (command.indexOf("POSITION") != -1) {
      queryMotorPosition(motorNum); // Query position for the specified motor
    } else {
      Serial.println("Invalid query type.");
      return; // Exit the function if the query type is invalid
    }
  } else if (command.startsWith("QUERY SENSOR")) {
    int sensorNumIndex = command.indexOf("SENSOR") + 6; // Find the index of "SENSOR" part in the command

    if (sensorNumIndex == -1) {
      Serial.println("Sensor number is missing.");
      return; // Exit the function if sensor number is missing
    }

    int sensorNum = command.substring(sensorNumIndex).toInt(); // Extract sensor number from command
    if (sensorNum < 1 || sensorNum > 6) {
      Serial.println("Invalid sensor number.");
      return; // Exit the function if the sensor number is invalid
    }

    querySensorSensitivity(sensorNum); // Query sensitivity for the specified sensor
  } else if (command.charAt(0) >= 'A' && command.charAt(0) <= 'G') {
    executeArray(command); // Call executeArray function for array commands
  } else if (command.startsWith("START DEMO")) {
    startDemoMode(); // Start demonstration mode
  } else if (command.startsWith("STOP DEMO")) {
    stopDemoMode(); // Stop demonstration mode
  } else {
    Serial.println("Invalid command.");
    return; // Exit the function if the command is invalid
  }
  
  printConfiguration(); // Print configuration after executing the command
}

void setMotorPosition(int motorNum, int position) {
  // Implement your logic to move the motor to the specified position here
  // For simplicity, we'll just set the position in the array
  motorPositions[motorNum - 1] = position;
}

void setMotorSpeed(int motorNum, int speed) {
  // Adjust speed within the range of 100 to 255
  speed = constrain(speed, 100, 255);
  // Set motor speed using PWM
  analogWrite(MOTOR_PINS[motorNum - 1], speed);
  // Update speed in the array
  motorSpeeds[motorNum - 1] = speed;
}

void setSensorSensitivity(int sensorNum, int sensitivity) {
  // Adjust sensitivity within the range of 1 to 10
  sensitivity = constrain(sensitivity, 1, 10);
  // Set sensitivity for the specified sensor
  sensorSensitivities[sensorNum - 1] = sensitivity;
}

void queryMotorPosition(int motorNum) {
  // Query position for the specified motor
  Serial.print("Position of Motor ");
  Serial.print(motorNum);
  Serial.print(": ");
  Serial.println(motorPositions[motorNum - 1]);
}

void queryMotorSpeed(int motorNum) {
  // Query speed for the specified motor
  Serial.print("Speed of Motor ");
  Serial.print(motorNum);
  Serial.print(": ");
  Serial.println(motorSpeeds[motorNum - 1]);
}

void querySensorSensitivity(int sensorNum) {
  // Print current sensitivity setting of the specified sensor
  Serial.print("Current sensitivity setting of Sensor ");
  Serial.print(sensorNum);
  Serial.print(": ");
  Serial.println(sensorSensitivities[sensorNum - 1]);
}

void printConfiguration() {
  // Print total configuration for all 6 motors and sensors
  Serial.println("Total Configuration:");
  for (int i = 0; i < 6; i++) {
    Serial.print("Motor ");
    Serial.print(i + 1);
    Serial.print(": Speed=");
    Serial.print(motorSpeeds[i]);
    Serial.print(", Position=");
    Serial.print(motorPositions[i]);
    Serial.print(", Sensor ");
    Serial.print(i + 1);
    Serial.print(": Sensitivity=");
    Serial.println(sensorSensitivities[i]);
  }
}

void executeArray(String command) {
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
    
    setMotorPosition(motorNumber, position); // Set position for motor i
    setMotorSpeed(motorNumber, speed); // Set speed for motor i
  }
}

void startDemoMode() {
  demoRunning = true;
  char demoArray;
  Serial.println("Demo Mode:");
  while (demoRunning) {
    for (int i = 0; i < 6 && demoRunning; i++) {
      demoArray = char('A' + i);
      Serial.print("Current Demo Array: ");
      Serial.println(demoArray);
      executeArray(String(demoArray)); // Execute arrays A-F sequentially
      delay(1000); // Delay 1 second
    }
    if (demoRunning) {
      demoArray = 'G';
      Serial.print("Current Demo Array: ");
      Serial.println(demoArray);
      executeArray(String(demoArray)); // Execute array G after arrays A-F
      delay(1000); // Delay 1 second
    }
  }
}

void stopDemoMode() {
  demoRunning = false;
  Serial.println("Demo Mode stopped.");
}
