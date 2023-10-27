#include <Arduino.h>
#include "display/display.h"

void setup()
{
  display_init();
}

void loop()
{
  display_loop();
  delay(5);
}

