#include "Wire.h"
WireClass Wire;

void WireClass::begin() {
}

void WireClass::setClock(long) {
}

void WireClass::write(uint8_t v) {
}

void WireClass::beginTransmission(int dst) {
}

void WireClass::endTransmission() {
}

void WireClass::requestFrom(int src, int len) {
}

int WireClass::available() {
    return 0;
}

uint8_t WireClass::read() {
    return 0;
}
