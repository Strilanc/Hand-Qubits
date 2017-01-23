//
// Detects electronic contact with an identical system on pin 2, and shares a single-byte message with the other side.
//
// Works by switching between listening and writing to the pin.
// Messages are encoded in the duration between pin toggles.
// Uses a magic byte and a crc byte to filter out noise.
//

#ifndef CONTACT_H
#define CONTACT_H

#include <stdint.h>

void contact_setup();
void contact_loop();

// Set the uint8_t value to send to the other side when in contact.
void contact_set_byte_to_send(uint8_t message_byte);
uint8_t contact_get_byte_to_send();

// Returns 255 if not in contact, else value being received from the other side.
uint8_t contact_get_current_other_message();

#endif

