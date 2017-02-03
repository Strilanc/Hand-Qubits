#include "ArduinoTestHarness.h"
#include <functional>
#include <map>
#include <queue>

uint64_t elapsed_micros = 0;
class DelayedCallback {
public:
    std::function<void(void)> callback;
    uint64_t time;

    bool operator<(const DelayedCallback& right) const {
        return time < right.time;
    }
};

std::priority_queue<DelayedCallback> callbacks{};
std::map<int, Pin> pins;

void pinMode(int pin, int mode) {
    pins[pin].mode = mode;
}

void digitalWrite(int pin, bool val) {
    if (pins[pin].mode == OUTPUT) {
        pins[pin].value = val ? 1023 : 0;
    } else {
        pins[pin].mode = val ? INPUT_PULLUP : INPUT;
    }
}

int random(int min_inclusive, int max_exclusive) {
    return min_inclusive;
}

int analogRead(int pin) {
    return pins[pin].value;
}

bool digitalRead(int pin) {
    return pins[pin].value >= 512;
}

uint32_t millis() {
    return (uint32_t)(elapsed_micros / 1000);
}

uint32_t micros() {
    return (uint32_t)elapsed_micros;
}

void test_harness_reset_arduino_state() {
    callbacks = {};
    pins = {};
    elapsed_micros = 0;
}

Pin test_harness_get_pin(int pin) {
    return pins[pin];
}

void test_harness_set_pin(int pin, bool val) {
    pins[pin].value = val ? 1023 : 0;
}

void test_harness_set_pin(int pin, int val) {
    pins[pin].value = val;
}

void test_harness_advance_time(uint32_t micros) {
    auto t = elapsed_micros + micros;
    while (!callbacks.empty() && callbacks.top().time <= t) {
        auto d = callbacks.top();
        callbacks.pop();
        elapsed_micros = d.time;
        d.callback();
    }
    elapsed_micros = t;
}

void test_harness_delayed_callback(uint32_t simulated_delay_micros, std::function<void(void)> callback) {
    callbacks.push({ callback, elapsed_micros + simulated_delay_micros });
}
