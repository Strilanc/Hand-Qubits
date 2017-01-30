#ifndef CODER_H
#define CODER_H

#include <stdint.h>

uint8_t crc8(uint8_t* bytes, int len, uint8_t gen = 0x31);

#define bitRead64(value, bit_position) ((((value) >> (bit_position)) & 1) != 0)
#define bitWrite64(variable, bit_position, new_bit_value) if (new_bit_value) { variable |= 1ULL << bit_position; } else { variable &= ~(1ULL << bit_position); }

#endif
