#include <Wire.h>
#include <math.h>

#include "contact.h"
#include "motion.h"
#include "quaternion.h"
#include "sound.h"

void setup() {
  Serial.begin(9600);
  //contact_setup();
  motion_setup();
  sound_setup();
}

void loop() {
  //contact_loop();
  motion_loop();
  sound_loop();
}

