#include <Arduino.h>
#include "config.h"
#include "Items.h"
#include "ExperienceSampler.h"

ExperienceSampler* es;

void setup() {
  // Serial.begin(115200);
  // while(!Serial);
  // Serial.println("Started Program");

  es = new ExperienceSampler();
}

void loop() {
  es->update();
}