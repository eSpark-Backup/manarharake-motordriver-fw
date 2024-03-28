#ifndef BOARD_H
#define BOARD_H

#define ARDUINO_BOARD 1
#define TEENSY_BOARD  2
#define ESP32_BOARD     3

// #define BOARD ARDUINO_BOARD
// #define BOARD TEENSY_BOARD
#define BOARD ESP32_BOARD

#define DEBUG 1

#if (DEBUG)
#define MH_LOG(x)       Serial.print(x)
#define MH_LOG_LN(x)    Serial.println(x)
#else
#define MH_LOG(x)       
#define MH_LOG_LN(x)    
#endif

#endif //BOARD_H