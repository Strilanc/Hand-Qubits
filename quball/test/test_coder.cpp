#include <stdio.h>
#include "common.h"
#include "../src/coder.h"
#include "../src/coder.cpp"

void coder_main() {
	test("crc8", []() {
		uint8_t data[] { 0xBA, 0xDF, 0x00, 0xD4, 0xAD, 0xEA, 0xDF, 0x00 };
		assert(crc8(data, 0) == 0);
		assert(crc8(data, sizeof(data)) == 13);
		assert(crc8(data, 2) == 85);
	});

  printf("CODER PASS\n");
}
