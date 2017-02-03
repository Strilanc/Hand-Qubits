#include "Wire.h"
WireClass Wire;

void WireClass::begin() {
}

void WireClass::setClock(long whatever) {
}

void WireClass::write(uint8_t v) {
    written.push_back(v);
}

void WireClass::beginTransmission(int dst) {
}

void WireClass::endTransmission() {
}

void WireClass::requestFrom(int src, int len) {
    requested += len;
}

int WireClass::available() {
    return to_read.size();
}

uint8_t WireClass::read() {
    uint8_t result = to_read.front();
    to_read.pop_front();
    return result;
}
