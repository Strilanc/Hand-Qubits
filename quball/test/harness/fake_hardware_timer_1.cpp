#include <functional>

#include "hardware_timer_1.h"
#include "ArduinoTestHarness.h"

static uint32_t version_counter = 0;

void on_callback(uint32_t expected_version, long microseconds, void(*callback)()) {
    if (expected_version != version_counter) {
        return;
    }
    callback();

    if (expected_version != version_counter) {
        return;
    }
    test_harness_delayed_callback(microseconds, [=]() {
        on_callback(expected_version, microseconds, callback);
    });
}

void stop_periodic_interrupt_timer_1() {
    version_counter++;
}

void start_periodic_interrupt_timer_1(long microseconds, void(*callback)()) {
    version_counter++;
    uint32_t expected_count = version_counter;
    test_harness_delayed_callback(microseconds, [=](){
        on_callback(expected_count, microseconds, callback);
    });
}
