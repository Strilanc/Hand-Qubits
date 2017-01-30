#ifndef UTIL_H
#define UTIL_H

#define bitRead64(value, bit_position) ((((value) >> (bit_position)) & 1) != 0)
#define bitWrite64(variable, bit_position, new_bit_value) if (new_bit_value) { variable |= 1ULL << (bit_position); } else { variable &= ~(1ULL << (bit_position)); }

#endif
