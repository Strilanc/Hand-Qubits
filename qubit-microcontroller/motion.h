//
// Reads gyro and accelerometer data from an MPU-6050.
// Accumulates the orientation and acceleration changes over several loops.
// Writes the deltas to Serial.
//

#ifndef MOTION_H
#define MOTION_H

#include <SoftwareSerial.h>

extern SoftwareSerial bluetoothSerial;

void motion_setup();
void motion_loop();

#endif
