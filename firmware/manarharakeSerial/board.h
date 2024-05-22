#ifndef BOARD_H
#define BOARD_H

#define ARDUINO_BOARD 1
#define TEENSY_BOARD  2
#define ESP32_BOARD     3

// #define BOARD ARDUINO_BOARD
#define BOARD TEENSY_BOARD
// #define BOARD ESP32_BOARD

#define DEBUG true
#define ADC_LOG_PRINT true

#if (BOARD == TEENSY_BOARD)
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