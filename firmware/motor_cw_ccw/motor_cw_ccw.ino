#define GPIO_EMC_00     55
#define GPIO_EMC_02     56
#define GPIO_EMC_03     57
#define GPIO_EMC_04     2
#define GPIO_EMC_05     3
#define GPIO_EMC_06     4
#define GPIO_EMC_07     33
#define GPIO_EMC_08     5
#define GPIO_EMC_09     72
#define GPIO_EMC_10     58
#define GPIO_EMC_11     59
#define GPIO_EMC_12     60
#define GPIO_EMC_13     61
#define GPIO_EMC_14     62
#define GPIO_EMC_15     63
#define GPIO_EMC_16     64
#define GPIO_EMC_17     65
#define GPIO_EMC_18     66
#define GPIO_EMC_19     67
#define GPIO_EMC_20     68
#define GPIO_EMC_21     69
#define GPIO_EMC_22     70
#define GPIO_EMC_23     71
#define GPIO_EMC_24     73

// #define GPIO_EMC_25     74
#define GPIO_EMC_25     53

#define GPIO_EMC_26     75

// #define GPIO_EMC_27     76
// #define GPIO_EMC_28     77

#define GPIO_EMC_27     49
#define GPIO_EMC_28     50

#define GPIO_EMC_29     78
#define GPIO_EMC_30     79
#define GPIO_EMC_31     29
#define GPIO_EMC_32     28
#define GPIO_EMC_33     80
#define GPIO_EMC_34     81
#define GPIO_EMC_35     82
#define GPIO_EMC_36     31
#define GPIO_EMC_37     30
#define GPIO_AD_B1_02   14
#define GPIO_AD_B1_03   15
#define GPIO_AD_B1_04   40
#define GPIO_AD_B1_05   41
#define GPIO_AD_B1_06   17

#define M1_PIN      GPIO_EMC_03
#define M2_PIN      GPIO_EMC_08
#define M3_PIN      GPIO_EMC_16
#define M4_PIN      GPIO_EMC_21
#define M5_PIN      GPIO_EMC_27 
#define M6_PIN      GPIO_EMC_32

#define M1_PIN2     GPIO_EMC_04
#define M2_PIN2     GPIO_EMC_09
#define M3_PIN2     GPIO_EMC_15
#define M4_PIN2     GPIO_EMC_20
#define M5_PIN2     GPIO_EMC_28
#define M6_PIN2     GPIO_EMC_33

#define M12_ENABLE_PIN  GPIO_EMC_00
#define M34_ENABLE_PIN  GPIO_EMC_13
#define M56_ENABLE_PIN  GPIO_EMC_25

#define M12_FAULT_PIN   GPIO_EMC_02
#define M34_FAULT_PIN   GPIO_EMC_14
#define M56_FAULT_PIN   GPIO_EMC_26

#define FSR3_PIN        GPIO_AD_B1_04

#define R1              arraySelectionPot
#define POT_R1_PIN      FSR3_PIN
#define DEFAULT_ACTIVATION 300
#define DEFAULT_ACTIVATION_R1   DEFAULT_ACTIVATION
#define HYSTERISIS      100

#define MH_LOG(x)           Serial.print(x)
#define MH_LOG_LN(x)        Serial.println(x)

class Potentiometer{
    private:
        int activationThreshold;
        int currentValue;
        int pin;
        bool incremented = false;
        int deactivationThreshold;
    public:
        bool toggle = false;
        Potentiometer(int _pin, int _activation)
        {
            pin = _pin;
            activationThreshold = _activation;
            deactivationThreshold =  activationThreshold - HYSTERISIS;
        }
        void initalize(){
            pinMode(pin, INPUT);
            MH_LOG_LN("POT Begin");
        }
        int getvalue(){
            return currentValue = analogRead(pin);
        }
        bool reachedActivation()
        {
            return (getvalue() >= activationThreshold) ? true : false;
        }
        bool reachedDeactivation()
        {
            return (getvalue() <= deactivationThreshold) ? true : false;
        }
        void resetFlag()
        {
            incremented = false;
        }
        void setFlag()
        {
            incremented = true;
        }
        bool incrementFlag()
        {
            return incremented;
        }
        void setActivation(int _activation)
        {
            activationThreshold = _activation;
            deactivationThreshold =  activationThreshold - HYSTERISIS;
            MH_LOG_LN("Activation Apllied");
        }
};



Potentiometer R1 = Potentiometer(POT_R1_PIN, DEFAULT_ACTIVATION_R1);
static int currentTime = 0;

void CW();
void CCW();
void setResistor(const String& command);


void potentiometerTask()
{
  static int adc_check_time = 0;
  /* array increment using pot */
  if(currentTime - adc_check_time > 250)
  {
  //   if(R2.reachedActivation())
  //   {
  //     if(!R2.incrementFlag())
  //     {
  //       R2.setFlag();
  //       /* increment the config by one */
  //       counter++;
  //       if(counter >= MOTOR_ARRAY_CONF_COUNT)
  //       {
  //         counter = 0;
  //       }
  //       MH_LOG("Cycle Config :");
  //       MH_LOG_LN(configCharArray[counter]);
  //     }
  //   }
  //   if(R2.reachedDeactivation())
  //   {
  //     if(R2.incrementFlag())
  //     {
  //       R2.resetFlag();
  //       MH_LOG_LN("Cycle Off!");
  //     }
  //   }

    /* array select pot */
    if(R1.reachedActivation())
    {
      if(!R1.incrementFlag())
      {
        R1.setFlag();
        if(R1.toggle)
        {
          R1.toggle = false;
          /*CCW*/
          CCW();
          // MH_LOG_LN("Exec Array counter");
        }
        else if(!R1.toggle)
        {
          R1.toggle = true;
          /*CW*/
          CW();
          // MH_LOG_LN("Exec Array Default");
        }
      }

    }
    if(R1.reachedDeactivation())
    {
      if(R1.incrementFlag())
      {
        R1.resetFlag();
      }
    }
    adc_check_time = currentTime;
  }

  

  /* printing adc values */
    
  // #if(ADC_LOG_PRINT)
  static int adcPrintTime = 0;

  if(currentTime - adcPrintTime > 300)
  {
    MH_LOG(0);
    MH_LOG(", ");

    // MH_LOG("Array Activation Pot ");
    MH_LOG(1024);
    MH_LOG(", ");
    
  //   ADC_LOG(R2.getvalue());
  //   ADC_LOG(", ");

  //   // MH_LOG("Array Activation Pot ");
    MH_LOG_LN(R1.getvalue());
    adcPrintTime = currentTime;
  }
  // #endif

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(M1_PIN, OUTPUT);
  pinMode(M1_PIN2, OUTPUT);
  Serial.println("Motor 1 initialized");

  pinMode(M2_PIN, OUTPUT);
  pinMode(M2_PIN2, OUTPUT);
  pinMode(M12_ENABLE_PIN, OUTPUT);
  digitalWrite(M12_ENABLE_PIN, HIGH);
  Serial.println("Motor 2 initialized");

  pinMode(M3_PIN, OUTPUT);
  pinMode(M3_PIN2, OUTPUT);
  Serial.println("Motor 3 initialized");

  pinMode(M4_PIN, OUTPUT);
  pinMode(M4_PIN2, OUTPUT);
  pinMode(M34_ENABLE_PIN, OUTPUT);
  digitalWrite(M34_ENABLE_PIN, HIGH);
  Serial.println("Motor 4 initialized");

  pinMode(M5_PIN, OUTPUT);
  pinMode(M5_PIN2, OUTPUT);
  Serial.println("Motor 5 initialized");

  pinMode(M6_PIN, OUTPUT);
  pinMode(M6_PIN2, OUTPUT);
  pinMode(M56_ENABLE_PIN, OUTPUT);
  digitalWrite(M56_ENABLE_PIN, HIGH);
  Serial.println("Motor 6 initialized");

}

void loop() {
  // put your main code here, to run repeatedly:
  currentTime = millis();

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');  // Read the command until newline character
    // for(int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++)
    // {
      if(command.startsWith("SET R"))
      {
        setResistor(command);
        command = "";
      }
    // }
    // if (command.charAt(0) >= CONFIG_A && command.charAt(0) <= CONFIG_G) {
    //     Manarharake::executeArray(command.charAt(0));  // Call executeArray function for array commands
    // }
  }


  potentiometerTask();

  
  // delay(5000);

  // delay(5000);
}

void CW()
{
  analogWrite(M5_PIN, 255);
  analogWrite(M5_PIN2, 0);

  analogWrite(M1_PIN, 255);
  analogWrite(M1_PIN2, 0);

  analogWrite(M2_PIN, 255);
  analogWrite(M2_PIN2, 0);

  analogWrite(M3_PIN, 255);
  analogWrite(M3_PIN2, 0);

  analogWrite(M4_PIN, 255);
  analogWrite(M4_PIN2, 0);

  analogWrite(M6_PIN, 255);
  analogWrite(M6_PIN2, 0);

  Serial.println("Moving CW");
}
void CCW()
{
  analogWrite(M5_PIN, 0);
  analogWrite(M5_PIN2, 255);

  analogWrite(M1_PIN, 0);
  analogWrite(M1_PIN2, 255);

  analogWrite(M2_PIN, 0);
  analogWrite(M2_PIN2, 255);

  analogWrite(M3_PIN, 0);
  analogWrite(M3_PIN2, 255);

  analogWrite(M4_PIN, 0);
  analogWrite(M4_PIN2, 255);


  analogWrite(M6_PIN, 0);
  analogWrite(M6_PIN2, 255);
  Serial.println("Moving CCW");
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
            MH_LOG("Resistor ");
            MH_LOG(resistorNum);
            MH_LOG(" set to value ");
            MH_LOG_LN(value);
            // setResisitorActivation(resistorNum, value);
            R1.setActivation(value);
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