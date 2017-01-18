#include "contact.h"
#include "motion.h"
#include "sound.h"

void setup() {
  Serial.begin(9600);

  contact_setup();
  motion_setup();
  sound_setup();
}

void loop() {
  contact_loop();
  motion_loop();
  sound_loop();

  read_any_commands_from_serial();
  set_ticking(contact_get_current_other_message() != -1);
}

void read_any_commands_from_serial() {
  while (Serial.available() > 0) {
    switch (Serial.read()) {
      case 'm':
        // Message.
        contact_set_byte_to_send(Serial.read());
        break;
        
      case 'r':
        // Ring.
        queue_tone(40, 10, 8, true);
        break;

      case 'b':
        // Beep beep!
        queue_tone(80, 120, 3, true);
        break;
    }
  }
}

