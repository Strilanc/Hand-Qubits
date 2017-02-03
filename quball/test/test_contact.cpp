#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "TestUtils.h"
#include "ArduinoTestHarness.h"
#include "contact.h"

void tick() {
    test_harness_advance_time(512);
}

void tick_until(std::function<bool(void)> predicate) {
    for (int i = 0; i < 10000 && !predicate(); i++) {
        tick();
    }
    assert(predicate());
};

// A manchester-encoded message.
static std::vector<int> msg42 {
    0, 0, 1, 1, // Pad.
    0, 0, 1, 1, // Magic.
    0, 0, 1, 1,
    1, 1, 0, 0,
    1, 1, 0, 0,
    1, 1, 0, 0,
    0, 0, 1, 1,
    1, 1, 0, 0,
    0, 0, 1, 1,
    1, 1, 0, 0, // Payload.
    0, 0, 1, 1,
    1, 1, 0, 0,
    0, 0, 1, 1,
    1, 1, 0, 0,
    0, 0, 1, 1,
    1, 1, 0, 0,
    1, 1, 0, 0,
    0, 0, 1, 1, // Check byte.
    0, 0, 1, 1,
    0, 0, 1, 1,
    1, 1, 0, 0,
    0, 0, 1, 1,
    1, 1, 0, 0,
    1, 1, 0, 0,
    0, 0, 1, 1
};

void contact_main() {
    test("contact_test", []() {
        contact_setup();

        // Setup.
        assert(contact_get_byte_to_send() == 0xFE);
        assert(contact_get_current_other_message() == 0xFF);
        contact_set_byte_to_send(42);
        assert(contact_get_byte_to_send() == 42);
        assert(contact_get_current_other_message() == 0xFF);

        // First listens, then eventually starts sending.
        assert(test_harness_get_pin(A0).mode == PIN_MODE_NOT_SET);
        tick();
        assert(contact_get_byte_to_send() == 42);
        assert(contact_get_current_other_message() == 0xFF);
        assert(test_harness_get_pin(A0).mode == INPUT);
        tick_until([]() { return test_harness_get_pin(A0).mode == OUTPUT; });
        assert(contact_get_current_other_message() == 0xFF);

        // Sends a properly encoded message representing the desired payload.
        for (int b : msg42) {
            bool is_on = test_harness_get_pin(A0).value > 512;
            bool expect_on = b == 0;
            assert(test_harness_get_pin(A0).mode == OUTPUT);
            assert(is_on == expect_on);
            tick();
        }
        assert(test_harness_get_pin(A0).mode == INPUT);
        assert(contact_get_current_other_message() == 0xFF);

        // Also hears the message.
        tick();
        tick();
        tick();
        tick();
        tick();
        tick();
        for (int b : msg42) {
            test_harness_set_pin(A0, b == 0);
            tick();
        }
        tick();
        assert(contact_get_current_other_message() == 42);

        // Starts sending soon after receiving.
        tick();
        tick();
        tick();
        tick();
        assert(test_harness_get_pin(A0).mode == OUTPUT);

        // Eventually forgets.
        assert(contact_get_current_other_message() == 42);
        tick_until([]() { return test_harness_get_pin(A0).mode == INPUT; });
        assert(contact_get_current_other_message() == 42);
        tick_until([]() { return test_harness_get_pin(A0).mode == OUTPUT; });
        assert(contact_get_current_other_message() == 0xFF);
    });

    test_without_reset("contact_fuzz", []() {
        for (int i = 0; i < 10000; i++) {
            if (test_harness_get_pin(A0).mode == INPUT) {
                test_harness_set_pin(A0, rand() % 1024);
            }
            tick();
        }
        
        // Still transitioning.
        tick_until([]() { return test_harness_get_pin(A0).mode == INPUT; });
        tick_until([]() { return test_harness_get_pin(A0).mode == OUTPUT; });
    });

    test_without_reset("contact_fuzz_manchester", []() {
        bool sending = false;
        for (int i = 0; i < 10000; i++) {
            if (test_harness_get_pin(A0).mode == INPUT && (i & 3) == 0) {
                sending = (rand() & 1) != 0;
            }
            test_harness_set_pin(A0, ((i & 2) != 0) ^ sending);
            tick();
        }

        // Still transitioning.
        tick_until([]() { return test_harness_get_pin(A0).mode == INPUT; });
        tick_until([]() { return test_harness_get_pin(A0).mode == OUTPUT; });
    });

    printf("CONTACT PASS\n");
}
