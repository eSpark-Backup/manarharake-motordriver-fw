#ifndef BOARD_H
#define BOARD_H

#define ARDUINO_BOARD 1
#define TEENSY_CUSTOM_BOARD  2
#define ESP32_BOARD     3
#define TEENSY_BOARD  4

// #define BOARD ARDUINO_BOARD
// #define BOARD TEENSY_CUSTOM_BOARD
#define BOARD TEENSY_BOARD
// #define BOARD ESP32_BOARD

#define DEBUG true
#define ADC_LOG_PRINT false

#if (BOARD == TEENSY_CUSTOM_BOARD || BOARD == TEENSY_BOARD)
#define GPIO_EMC_00     55          //ENABLE_1_2
#define GPIO_EMC_02     56          //FAULT_1_2
#define GPIO_EMC_03     57          //MOTOR_1_F
#define GPIO_EMC_04     2           //MOTOR_1_B
#define GPIO_EMC_05     3           //ENC_M1_A
#define GPIO_EMC_06     4           //ENC_M1_B
#define GPIO_EMC_07     33          //ENC_M1_Z
#define GPIO_EMC_08     5           //MOTOR_2_F
#define GPIO_EMC_09     72          //MOTOR_2_B
#define GPIO_EMC_10     58          //ENC_M2_A
#define GPIO_EMC_11     59          //ENC_M2_B
#define GPIO_EMC_12     60          //ENC_M2_Z
#define GPIO_EMC_13     61          //ENABLE_3_4
#define GPIO_EMC_14     62          //FAULT_3_4
#define GPIO_EMC_15     63          //MOTOR_3_F
#define GPIO_EMC_16     64          //MOTOR_3_B
#define GPIO_EMC_17     65          //ENC_M3_A
#define GPIO_EMC_18     66          //ENC_M3_B
#define GPIO_EMC_19     67          //ENC_M3_Z
#define GPIO_EMC_20     68          //MOTOR_4_F
#define GPIO_EMC_21     69          //MOTOR_4_B
#define GPIO_EMC_22     70          //ENC_M4_A
#define GPIO_EMC_23     71          //ENC_M4_B
#define GPIO_EMC_24     73          //ENC_M4_Z

// #define GPIO_EMC_25     74
#define GPIO_EMC_25     53          //ENABLE_5_6

#define GPIO_EMC_26     75          //FAULT_5_6

// #define GPIO_EMC_27     76
// #define GPIO_EMC_28     77
    
#define GPIO_EMC_27     49          //MOTOR_5_F
#define GPIO_EMC_28     50          //MOTOR_5_B

#define GPIO_EMC_29     78          //ENC_M5_A
#define GPIO_EMC_30     79          //ENC_M5_B
#define GPIO_EMC_31     29          //ENC_M5_Z
#define GPIO_EMC_32     28          //MOTOR_6_F
#define GPIO_EMC_33     80          //MOTOR_6_B
#define GPIO_EMC_34     81          //ENC_M6_A
#define GPIO_EMC_35     82          //ENC_M6_B
#define GPIO_EMC_36     31          //ENC_M6_Z
#define GPIO_EMC_37     30          //TEST_LED
#define GPIO_AD_B1_02   14          //FSR1
#define GPIO_AD_B1_03   15          //FSR2
#define GPIO_AD_B1_04   40          //FSR3
#define GPIO_AD_B1_05   41          //FSR4
#define GPIO_AD_B1_06   17          //FSR5

#define TEST_LED        GPIO_EMC_37
#endif

#if (DEBUG)
#define MH_LOG(x)           Serial.print(x)
#define MH_LOG_LN(x)        Serial.println(x)
#else
#define MH_LOG(x)       
#define MH_LOG_LN(x)    
#endif

#if (ADC_LOG_PRINT)
#define ADC_LOG(x)          Serial.print(x)
#define ADC_LOG_LN(x)       Serial.println(x)
#else
#define ADC_LOG(x)   
#define ADC_LOG_LN(x)
#endif


#endif //BOARD_H