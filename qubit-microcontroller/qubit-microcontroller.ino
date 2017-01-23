#include "contact.h"
#include "motion.h"
#include "sound.h"

#define LED_PIN 13

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);

  contact_setup();
  motion_setup();
  sound_setup();
}

void loop() {
  contact_loop();
  motion_loop();
  sound_loop();

  read_any_commands_from_serial();

  bool is_in_contact = contact_get_current_other_message() != 0xFF;
  set_ticking(is_in_contact);
  digitalWrite(LED_PIN, is_in_contact);
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

