#ifndef MOTOR_H
#define MOTOR_H

class Motor {
private:
    int speed;
    int position;
    int sensorSensitivity;
    int pin;

public:
    Motor(int initialSpeed, int initialPosition, int initialSensorSensitivity, int pin);

    void setSpeed(int newSpeed);
    void setPosition(int newPosition);
    void setSensorSensitivity(int newSensitivity);
    int getSpeed();
    int getPosition();
    int getSensorSensitivity();
};

#endif