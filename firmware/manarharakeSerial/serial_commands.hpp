#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

#include <Arduino.h>

struct SerialCommand{
  char* commandString;
  void(*commandCallback)(const String& command);
};


#endif //SERIAL_COMMANDS_H