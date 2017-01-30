// This is a test harness that stands in for the normal Arduino stuff.

#ifndef WIRE_H
#define WIRE_H

#include <stdint.h>

class WireClass {
public:
    static void begin();
    static void setClock(long);
    static void write(uint8_t v);
    static void beginTransmission(int);
    static void endTransmission();
    static void requestFrom(int, int);
    static int available();
    static uint8_t read();
};

extern WireClass Wire;

#endif
