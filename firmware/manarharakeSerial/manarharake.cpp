#include "manarharake.hpp"
#include "board.h"

#include <Arduino.h>
#include <EEPROM.h>
#include "motor_control.hpp"
#include "serial_commands.hpp"
#include "encoder.hpp"
#include "rtc_mem.hpp"


// Define 4 32-bit registers and 128bit motorData struct
#if(BOARD == ESP32_BOARD)
RTC_DATA_ATTR static uint32_t registers[4] = {0};
#elif(BOARD == TEENSY_BOARD)
uint32_t *NVRAM_UINT32 ((uint32_t *)0x400D4100);
uint32_t *registers = NVRAM_UINT32;
#endif
MotorData rtcMotorDataBuffer;

// Definition FSR object to be assigned to pointer
ForceSensitiveResistor fsr[FSR_COUNT] = {
  ForceSensitiveResistor(FSR1_PIN),
  ForceSensitiveResistor(FSR2_PIN),
  ForceSensitiveResistor(FSR3_PIN),
  ForceSensitiveResistor(FSR4_PIN),
  ForceSensitiveResistor(FSR5_PIN),
};

// Instantiate the RotaryEncoder class for each encoder
RotaryEncoder encoder1(ENCODER1_PINA, ENCODER1_PINB);
RotaryEncoder encoder2(ENCODER2_PINA, ENCODER2_PINB);
RotaryEncoder encoder3(ENCODER3_PINA, ENCODER3_PINB);
RotaryEncoder encoder4(ENCODER4_PINA, ENCODER4_PINB);
RotaryEncoder encoder5(ENCODER5_PINA, ENCODER5_PINB);
RotaryEncoder encoder6(ENCODER6_PINA, ENCODER6_PINB);

// Definition of static member variable
Motor Manarharake::motor[MOTOR_COUNT] = {
    Motor({DEFAULT_SPEED, DEFAULT_POSITION, DEFAULT_SENSITIVITY, M1_PIN, M1_PIN2, M12_ENABLE_PIN}, &fsr[0], &encoder1), // Motor 1
    Motor({DEFAULT_SPEED, DEFAULT_POSITION, DEFAULT_SENSITIVITY, M2_PIN, M2_PIN2, M12_ENABLE_PIN}, &fsr[1], &encoder2), // Motor 2
    Motor({DEFAULT_SPEED, DEFAULT_POSITION, DEFAULT_SENSITIVITY, M3_PIN, M3_PIN2, M34_ENABLE_PIN}, &fsr[2], &encoder3), // Motor 3
    Motor({DEFAULT_SPEED, DEFAULT_POSITION, DEFAULT_SENSITIVITY, M4_PIN, M4_PIN2, M34_ENABLE_PIN}, &fsr[3], &encoder4), // Motor 4
    Motor({DEFAULT_SPEED, DEFAULT_POSITION, DEFAULT_SENSITIVITY, M5_PIN, M5_PIN2, M56_ENABLE_PIN}, &fsr[4], &encoder5), // Motor 5
    Motor({DEFAULT_SPEED, DEFAULT_POSITION, DEFAULT_SENSITIVITY, M6_PIN, M6_PIN2, M56_ENABLE_PIN}, &fsr[4], &encoder6)  // Motor 6
};

//Definition of demoState
bool Manarharake::demo_state = false;

void sleepFcn(const String& command) //For testing RTC logic
{
  #if(BOARD == ESP32_BOARD)
  esp_deep_sleep(10000000);
  #endif
}


// Command List
SerialCommand commands[] = {
  {SET_MOTOR_CMD      , Manarharake::setMotor           },
  {SET_SENSOR_CMD     , Manarharake::setSensor          },
  {QUERY_MOTOR_CMD    , Manarharake::queryMotor         },
  {QUERY_SENSOR_CMD   , Manarharake::querySensor        },
  {SET_RESISTANCE_CMD , Manarharake::setResistor        },
  {START_DEMO_CMD     , Manarharake::startDemo          },
  {STOP_DEMO_CMD      , Manarharake::stopDemo           },
  {SAVE_CONFIG_CMD    , Manarharake::saveConfiguration  },
  {LOAD_CONFIG_CMD    , Manarharake::loadConfiguration  },
  {"SLEEP"            , sleepFcn                        }, //For testing RTC Data saving logic
};

void Manarharake::initalize()
{
    Serial.begin(BAUDRATE); 
    #if(BOARD == TEENSY_BOARD)
    EEPROM.begin();
    SNVS_LPCR |= (1 << 24); //Enable NVRAM - documented in SDK[/B]  
    if(CrashReport)
    {
      MH_LOG("Reset Reason : "); 
      MH_LOG_LN(CrashReport); //TODO: Check crashreport then load from RTC if required
      loadMotorData(registers, (uint32_t *)&rtcMotorDataBuffer);
      for(int i = 0; i < MOTOR_COUNT; i++)
      {
        int *encoderVal = (int*)malloc(sizeof(int));
        getMotorData(&rtcMotorDataBuffer, i, encoderVal);
        int _encPos = *encoderVal;
        MH_LOG("Encoder Val = ");
        MH_LOG(_encPos);
        motor[i].setEncoder(_encPos);
        free(encoderVal);
      }
    }
    #elif(BOARD == ESP32_BOARD)
    EEPROM.begin(EEPROM_SIZE_BYTES);

    MH_LOG("Reset Reason : ");
    MH_LOG( esp_reset_reason() ); 

    if(esp_reset_reason())
    {
      MH_LOG("Deep Sleep");
      loadMotorData(registers, (uint32_t *)&rtcMotorDataBuffer);
      for(int i = 0; i < MOTOR_COUNT; i++)
      {
        int *encoderVal = (int*)malloc(sizeof(int));
        getMotorData(&rtcMotorDataBuffer, i, encoderVal);
        int _encPos = *encoderVal;
        motor[i].setEncoder(_encPos);
        free(encoderVal);
        MH_LOG("Encoder Val = ");
        MH_LOG(_encPos);
      }
    }
    #endif

    for(int i = 0; i < MOTOR_COUNT; i++)
    {
      motor[i].begin();
    }

    loadConfiguration("NULL");
    MH_LOG_LN("Controller Started");
}

unsigned long lastExecutionTime = 0;
unsigned long lastPrintTime = 0;
unsigned long currentTime;
char demoArray = 'A';
const char configCharArray[MOTOR_ARRAY_CONF_COUNT] = {CONFIG_A, CONFIG_B, CONFIG_C, CONFIG_D, CONFIG_E, CONFIG_F, CONFIG_G};


void Manarharake::loop()
{
    static int caseDemoArray = 0;
    static int counter = 0;
    currentTime = millis();

    if( currentTime - lastPrintTime >= 500)
    {
      Manarharake::printConfiguration(); //TODO: Add timed ouptut so that print is not so fast
      lastPrintTime = currentTime;
    }

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

    Manarharake::maintainPos();
    Manarharake::checkFsrThreshold();


    if(demo_state)
    {

      switch (caseDemoArray)
      {
        case DEMO_STATE_1:
          demoArray = configCharArray[counter];
          Manarharake::executeArray(demoArray); // Execute arrays A-F sequentially
          counter ++;
          if(counter >= MOTOR_ARRAY_CONF_COUNT)
            counter = 0;
          caseDemoArray = DEMO_STATE_2;
          lastExecutionTime = currentTime;
        break;
        case DEMO_STATE_2:
          if (currentTime - lastExecutionTime >= 1000) { 
            caseDemoArray = DEMO_STATE_3;
          }
        break;
        case DEMO_STATE_3:
          Manarharake::executeArray(CONFIG_G); // Execute arrays A-F sequentially
            caseDemoArray = DEMO_STATE_4;
            lastExecutionTime = currentTime;
        break;
        case DEMO_STATE_4:
          if (currentTime - lastExecutionTime>= 1000) { 
            caseDemoArray = DEMO_STATE_1;
          }
        break;
      }
    }

}

void Manarharake::checkFsrThreshold()
{
    for(int i = 0; i < MOTOR_COUNT; i++)
    {
      motor[i].checkFsr();
    }
}

void Manarharake::maintainPos()
{
  for(int i = 0; i < MOTOR_COUNT; i++)
  {
    setMotorData(&rtcMotorDataBuffer, i, motor[i].getEncoder());
    saveMotorData(registers, (uint32_t *)&rtcMotorDataBuffer);
    motor[i].maintainPosition();
  }
}

void Manarharake::printConfiguration() 
{
  // Print total configuration for all 6 motors and sensors
  // MH_LOG_LN("Total Configuration:");
  static int i = 0;
    MH_LOG(i + 1);
    MH_LOG(": Speed = ");
    MH_LOG(motor[i].getSpeedSetting());
    MH_LOG("\t, Pos =");
    MH_LOG(motor[i].getPositionSetting());
    MH_LOG("\t, Sensor ");
    MH_LOG(i + 1);
    MH_LOG(": Current Sens Lvl = ");
    MH_LOG(motor[i].readCurrentFsrVal()); //Read current Analog level
    MH_LOG("\t: Sens =");
    MH_LOG(motor[i].getSensorSensitivity());
    MH_LOG("\t: Current Angle = ");
    MH_LOG(motor[i].getMotorAngle());
    MH_LOG("\t Motor Current Rev = ");
    MH_LOG(motor[i].getCurrentPostion());
    MH_LOG("\t Halt : ");
    MH_LOG_LN(motor[i].getHaltState() ? "true" : "false");
  i++;
  if(i >= MOTOR_COUNT)
  {
    MH_LOG_LN("Total Configuration:");
    i = 0;
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
      case M0: motorNumber = MOTOR_6; break;
      case M1: motorNumber = MOTOR_5; break;
      case M2: motorNumber = MOTOR_1; break;
      case M3: motorNumber = MOTOR_2; break;
      case M4: motorNumber = MOTOR_3; break;
      case M5: motorNumber = MOTOR_4; break;
    }
    
    motor[motorNumber].setSpeed(speed); // Set speed for motor i
    motor[motorNumber].setPosition(position); // Set speed for motor i
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

      int speed = motor[i].getSpeedSetting();
      int position = motor[i].getPositionSetting();
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
  // if (demoTaskHandle != NULL) {
    // vTaskResume(printTaskHandle);
    // vTaskSuspend(demoTaskHandle);
    // vTaskSuspend(fsrTaskHandle);
    demo_state = false;
    loadConfiguration("NULL");
    for(int i = 0; i < MOTOR_COUNT; i++)
    {
      motor[i].setHaltState(false);
    }
  // }
}

void Manarharake::startDemo(const String& command)
{
  // vTaskSuspend(printTaskHandle);

  // vTaskResume(demoTaskHandle);
  // vTaskResume(fsrTaskHandle);

  demo_state = true;
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
            MH_LOG_LN(motor[motorNum - 1].getSpeedSetting());
        } else if (queryType.equals(PARAM2)) {
            MH_LOG_LN(motor[motorNum - 1].getPositionSetting());
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
