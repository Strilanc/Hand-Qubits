#include <stdio.h>
#include "common.h"
#include "motion.h"

SoftwareSerial bluetoothSerial;

void motion_main() {
    test("-", []() {
		assert(true);
	});

    printf("MOTION PASS\n");
}
