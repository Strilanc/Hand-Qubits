#include "coder.h"

uint8_t crc8(uint8_t* bytes, int len, uint8_t gen) {
  uint8_t crc = 0;
  for (int i = 0; i < len; i++) {
    crc ^= bytes[i];
    for (int j = 0; j < 8; j++) {
		  int b = crc & 0x80;
		  crc <<= 1;
      if (b) {
        crc ^= gen;
      }
    }
  }
  return crc;
} 
