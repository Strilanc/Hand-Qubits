// This is a test harness that stands in for the normal Arduino stuff.

#ifndef WIRE_H
#define WIRE_H

#include <stdint.h>
#include <deque>

class WireClass {
public:
    uint64_t requested = 0;
    std::deque<uint8_t> written {};
    std::deque<uint8_t> to_read {};

    void begin();
    void setClock(long);
    void write(uint8_t v);
    void beginTransmission(int dst);
    void endTransmission();
    void requestFrom(int src, int len);
    int available();
    uint8_t read();
};

extern WireClass Wire;

#endif
