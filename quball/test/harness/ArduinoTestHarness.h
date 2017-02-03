// This is a test harness that stands in for the normal Arduino stuff.

#ifndef ARDUINO_TEST_HARNESS_H
#define ARDUINO_TEST_HARNESS_H

#include "Arduino.h"
#include <functional>

class Pin {
public:
    int value;
    int mode;
};

void test_harness_reset_arduino_state();
Pin test_harness_get_pin(int pin);
void test_harness_set_pin(int pin, bool val);
void test_harness_advance_time(uint32_t micros);
void test_harness_delayed_callback(uint32_t simulated_delay_micros, std::function<void(void)> callback);

#endif
