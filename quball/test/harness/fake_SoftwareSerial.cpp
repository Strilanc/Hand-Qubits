#include "SoftwareSerial.h"

void SoftwareSerial::write(uint8_t val) {
    test_harness_written.push_back(val);
}

void SoftwareSerial::write(uint8_t* buf, int len) {
    for (int i = 0; i < len; i++) {
        test_harness_written.push_back(buf[i]);
    }
}
