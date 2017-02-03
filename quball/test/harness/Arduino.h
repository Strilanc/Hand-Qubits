// This is a test harness that stands in for the normal Arduino stuff.

#ifndef ARDUINO_H
#define ARDUINO_H

#include <cinttypes>

#define OUTPUT 101
#define INPUT 102
#define INPUT_PULLUP 103

#define A0 200
#define A1 201
#define A2 202
#define A3 203
#define A4 204
#define A5 205
#define A6 206

#define HIGH true
#define LOW false

#define byte uint8_t

#define bitRead(value, bit_position) ((((value) >> (bit_position)) & 1) != 0)
#define bitWrite(target, bit, val) if (val) { target |= 1 << (bit); } else { target &= ~(1 << (bit)); }

void pinMode(int pin, int mode);
void digitalWrite(int pin, bool val);
int random(int min_inclusive, int max_exclusive);
bool digitalRead(int pin);
int analogRead(int pin);
uint32_t millis();
uint32_t micros();

template<typename T>
T max(T left, T right) {
    return left > right ? left : right;
}

template<typename T>
T min(T left, T right) {
    return left < right ? left : right;
}

template<typename T>
T abs(T val) {
    return val > 0 ? val : -val;
}

#endif
