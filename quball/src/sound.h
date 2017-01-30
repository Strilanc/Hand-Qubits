//
// Controls a piezo speaker on A3.
//

#ifndef SOUND_H
#define SOUND_H

void queue_tone(int tone_millis, int quiet_millis, int repeats = 1, bool flush = false);
void sound_loop();
void sound_setup();
void set_ticking(bool ticking);

#endif
