#ifndef FORCE_SENSITIVE_H
#define FORCE_SENSITIVE_H

#define FSR_COUNT       5
#define MIN_FSR_SENS    1
#define MAX_FSR_SENS    10

// Pin definition for ESP32
#define FSR1_PIN        36
#define FSR2_PIN        39
#define FSR3_PIN        34
#define FSR4_PIN        35
#define FSR5_PIN        32

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
        bool sensitivityReached();
        void setFsrPin(int pin);
};

#endif //FORCE_SENSITIVE_H