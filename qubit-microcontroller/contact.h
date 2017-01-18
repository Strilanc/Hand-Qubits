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

void contact_loop();
void contact_setup();
void contact_set_byte_to_send(uint8_t message_byte);
int contact_get_current_other_message();

#endif

