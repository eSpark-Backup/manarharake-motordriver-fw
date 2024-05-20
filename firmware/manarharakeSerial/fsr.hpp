#ifndef FORCE_SENSITIVE_H
#define FORCE_SENSITIVE_H
#include "board.h"

#define FSR_COUNT       3
#define MIN_FSR_SENS    1
#define MAX_FSR_SENS    10

#define MIN_10_BIT      0
#define MAX_10_BIT      1023

#define MIN_12_BIT      0
#define MAX_12_BIT      4096

#define MIN_FSR_SENS_RAW    MIN_10_BIT
#define MAX_FSR_SENS_RAW    MAX_10_BIT

#if (BOARD == ESP32_BOARD)
// Pin definition for ESP32
#define FSR1_PIN        36
#define FSR2_PIN        39
#define FSR3_PIN        34
#define FSR4_PIN        35
#define FSR5_PIN        32
#endif

#if (BOARD == TEENSY_BOARD)
// Pin definition for Teensy
#define FSR1_PIN        GPIO_AD_B1_02
#define FSR2_PIN        GPIO_AD_B1_03
#define FSR3_PIN        GPIO_AD_B1_04
#define FSR4_PIN        GPIO_AD_B1_05
#define FSR5_PIN        GPIO_AD_B1_06
#endif

#define DEFAULT_SENSITIVITY 1


class ForceSensitiveResistor{
    private:
        int sensitivtyLevel;
        int sensitivityRaw;
        int currentSensitivity;
        int fsrPin;
    public:
        ForceSensitiveResistor(int pin);
        int getFsrSenLvl();
        void setFsrSenLvl(int value);

        int readFsrSens();
        int readFsrSensMapped();
        bool sensitivityReached();
        void setFsrPin(int pin);
};

#endif //FORCE_SENSITIVE_H