#include <stdio.h>
#include "TestUtils.h"
#include "util.h"
#include "ArduinoTestHarness.h"
#include "hardware_timer_1.h"

static int count = 0;
void increment_count() {
    count++;
}

void harness_main() {
    test("elapsed_time", []() {
        assert(millis() == 0);
        assert(micros() == 0);

        test_harness_advance_time(1000);
        assert(millis() == 1);
        assert(micros() == 1000);

        test_harness_advance_time(1500);
        assert(millis() == 2);
        assert(micros() == 2500);

        test_harness_advance_time(0xFFFFFFFF);
        assert(millis() == 4294969);
        assert(micros() == 2499);

        test_harness_reset_arduino_state();
        assert(millis() == 0);
        assert(micros() == 0);
    });

    test("pin_output", []() {
        pinMode(13, OUTPUT);
        assert(test_harness_get_pin(13).value == 0);

        digitalWrite(13, true);
        assert(test_harness_get_pin(13).value == 1023);

        digitalWrite(13, false);
        assert(test_harness_get_pin(13).value == 0);

        digitalWrite(13, true);
        test_harness_reset_arduino_state();
        assert(test_harness_get_pin(13).value == 0);
        assert(test_harness_get_pin(13).mode == 0);
    });

    test("pin_input", []() {
        pinMode(13, INPUT);
        pinMode(12, INPUT);
        assert(!digitalRead(12));
        assert(!digitalRead(13));

        test_harness_set_pin(13, true);
        assert(!digitalRead(12));
        assert(digitalRead(13));

        test_harness_set_pin(12, true);
        assert(digitalRead(12));
        assert(digitalRead(13));

        test_harness_set_pin(13, false);
        assert(digitalRead(12));
        assert(!digitalRead(13));
    });

    test("periodic_timer", []() {
        count = 0;
        start_periodic_interrupt_timer_1(25, increment_count);

        assert(count == 0);
        test_harness_advance_time(24);
        assert(count == 0);
        test_harness_advance_time(1);
        assert(count == 1);
        test_harness_advance_time(25);
        assert(count == 2);
        test_harness_advance_time(100);
        assert(count == 6);

        stop_periodic_interrupt_timer_1();
        test_harness_advance_time(100);
        assert(count == 6);
    });

    printf("HARNESS PASS\n");
}
