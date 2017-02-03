#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <string>

void test(std::string name, void(*func)(void));
void assert(bool truth);

#endif // TEST_UTILS_H
