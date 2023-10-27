#include <Arduino.h>
#include <Ticker.h>
#include "config.h"
#include "display/display.h"

Ticker ticker;

void onTimer()
{
  Serial.printf("SRAM free size: %d\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
}

void setup()
{
  pinMode(SUICIDE_PIN, OUTPUT);
  digitalWrite(SUICIDE_PIN, HIGH);

  Serial.begin(115200);
  ticker.attach(1, onTimer);
  
  display_init();
  
  Serial.println("Setup done");
}

void loop()
{
  display_loop();
  delay(5);
}

