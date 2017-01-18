#ifndef CODER_H
#define CODER_H

#include <stdint.h>

uint8_t crc8(uint8_t* bytes, int len, uint8_t gen = 0x31);

#endif
