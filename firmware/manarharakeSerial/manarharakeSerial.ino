#include <Arduino.h>
#include "manarharake.hpp"

Manarharake mhController;

void setup() {
  mhController.initalize();
}

void loop() {
  mhController.loop();
  delay(1);
}



