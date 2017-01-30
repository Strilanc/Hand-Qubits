#include "common.h"
#include <string>
#include <stdio.h>
#include <exception>

static std::string cur_name = "";
static int cur_assert = 0;
void coder_main();
void quaternion_main();
void contact_main();
void motion_main();

int main() {
	quaternion_main();
    coder_main();
    contact_main();
    motion_main();
    printf("ALL PASS\n");
	return 0;
}

void test(std::string name, void(*func)(void)) {
	cur_assert = 0;
	cur_name = name;
	func();
}

void assert(bool truth) {
	cur_assert++;
	if (!truth) {
		throw "assert in " + cur_name + " at #" + std::to_string(cur_assert);
	}
}
