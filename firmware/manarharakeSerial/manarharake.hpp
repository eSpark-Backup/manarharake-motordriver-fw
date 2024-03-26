#ifndef MANARHARAKE_H
#define MANARHARAKE_H

#define CONFIG_A   'A' 
#define CONFIG_B   'B' 
#define CONFIG_C   'C' 
#define CONFIG_D   'D' 
#define CONFIG_E   'E' 
#define CONFIG_F   'F' 
#define CONFIG_G   'G' 

#define MOTOR_1     1
#define MOTOR_2     2
#define MOTOR_3     3
#define MOTOR_4     4
#define MOTOR_5     5
#define MOTOR_6     6

#define SPEED_OFFSET 10
#define POSITION_LOW_OFFSET 20
#define POSITION_HIGH_OFFSET 30
#define SENSOR_SENSITIVITY_OFFSET 40

#define MOTOR_COUNT sizeof(motors)/sizeof(motors[0])


class Manarharake{
    private:
    public:
        void initalize();
};




#endif //MANARHARAKE_H