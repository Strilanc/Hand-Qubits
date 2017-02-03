// This is a test harness that stands in for the normal Arduino stuff.

#ifndef SOFTWARE_SERIAL_H
#define SOFTWARE_SERIAL_H

#include <stdint.h>
#include <deque>

class SoftwareSerial {
public:
    void write(uint8_t val);
    void write(uint8_t* buf, int len);

    std::deque<uint8_t> test_harness_written;
};

#endif
