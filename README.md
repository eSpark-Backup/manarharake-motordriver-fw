# Motor Control Board Firmware Development
#### Client: Manar Harake

## Overview
The firmware controls a system consisting of multiple DC motors and sensors. It interprets and executes serial commands to adjust motor positions, motor speeds, sensor sensitivities, and to select or execute predefined motor position arrays. Additionally, it incorporates an analog control mechanism using inputs from two variable resistors for interactive array control.

### [PRD Link](https://github.com/eSpark-Consultants/manarharake-motordriver-fw/blob/main/docs/PRD/Firmware%20Requirements%20-%20Motor%20driver%20PCB.pdf) 

### Requirements different from the PRD
- It was decided that we will use revolutions as our primary unit. This means that if a command sent indicates the motor should move 300, it means 300 revolutions. The max revolutions expected from a motor in one sequence are shown in table below: (Although these values may indicate the maximum revolution of a motor, the firmware should not block the user from entering a higher value in future if required and hence the implementation should be independent of maximum limit). Please further note that all values will be absolute (We can round off the fractional values by discarding the fractional part).
![image](https://github.com/eSpark-Consultants/manarharake-motordriver-fw/assets/76909080/9b59c21c-59ca-49a3-9732-a44430ba36af)
