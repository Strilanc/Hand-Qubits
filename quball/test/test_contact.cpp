#include <stdio.h>
#include <vector>
#include "TestUtils.h"
#include "ArduinoTestHarness.h"
#include "contact.h"

void contact_main() {
    test("contact_test", []() {
        auto tick = []() {test_harness_advance_time(512); };
        auto tick_until = [&](std::function<bool(void)> predicate) {
            for (int i = 0; i < 10000 && !predicate(); i++) {
                tick();
            }
            assert(predicate());
        };

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

        // Sends a manchester-encoded message representing the desired payload.
        std::vector<int> msg{
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
        for (int b : msg) {
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
        for (int b : msg) {
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

    printf("CONTACT PASS\n");
}
